#include "backend/player_command.hpp"
#include "core/data/session.hpp"
#include "core/settings/notifications.hpp"
#include "gui.hpp"
#include "hooking.hpp"
#include "services/bad_players/bad_players.hpp"
#include "services/friends/friends_service.hpp"
#include "services/gui/gui_service.hpp"
#include "services/known_players.hpp"
#include "services/players/player_service.hpp"
#include "util/notify.hpp"
#include "util/player.hpp"

namespace big
{
	inline bool is_spoofed_host_token(uint64_t token)
	{
		if (token < 1000'000'000'000)
			return true;

		return false;
	}

	void* hooks::assign_physical_index(CNetworkPlayerMgr* netPlayerMgr, CNetGamePlayer* player, uint8_t new_index)
	{
		const auto* net_player_data = player->get_net_data();

		auto rockstar_id = net_player_data->m_gamer_handle.m_rockstar_id;
		auto player_name = net_player_data->m_name;
		auto host_token  = net_player_data->m_host_token;

		if (new_index == static_cast<uint8_t>(-1))
		{
			if (*g_pointers->m_gta.m_is_session_started && g_session.next_host_list.current_host
			    && g_session.next_host_list.current_host->id() == player->m_player_id)
			{
				g_session.next_host_list.current_host       = nullptr;
				g_session.next_host_list.determine_new_host = true;
			}

			g_player_service->player_leave(player);
			g_session.next_host_list.delete_plyr(player->m_player_id);

			if (net_player_data)
			{
				if (g_notifications.player_leave.log)
					LOGF(INFO, "Player left '{}', slot #{}. RID: {}", player_name, (int)player->m_player_id, rockstar_id);

				if (g_notifications.player_leave.notify)
					g_notification_service->push("Player Left", std::vformat("{} freeing slot", std::make_format_args(player_name)));
			}

			return g_hooking->get_original<hooks::assign_physical_index>()(netPlayerMgr, player, new_index);
		}

		const auto result = g_hooking->get_original<hooks::assign_physical_index>()(netPlayerMgr, player, new_index);

		auto plyr = g_player_service->player_join(player);

		if (plyr && plyr->is_host())
			g_session.next_host_list.current_host = plyr;
		else
			g_session.next_host_list.insert_plyr(player->m_player_id, host_token, player_name);

		if (host_token < g_session.smallest_host_token)
		{
			g_session.smallest_host_token       = host_token;
			g_session.smallest_host_token_owner = player_name;
		}

		if (net_player_data)
		{
			if (g_notifications.player_join.log)
				LOGF(INFO,
				    "Player joined '{}'{}, slot #{}. RID: {}",
				    player_name,
				    player->is_host() ? "(host)" : "",
				    (int)player->m_player_id,
				    rockstar_id);
			if (g_notifications.player_join.notify)
				g_notification_service->push("Player Joined", std::vformat("{} taking slot", std::make_format_args(player_name)));

			if (player->m_player_id != self::id)
				g_fiber_pool->queue_job([plyr, rockstar_id, player_name, host_token] {
					if (plyr && plyr->is_valid())
					{
						if (*g_pointers->m_gta.m_is_session_started)
						{
							if (known_player_nm::is_known(rockstar_id))
								plyr->is_known_player = true;

							if (bad_players_nm::is_blocked(rockstar_id))
							{
								auto str = get_blocked_player_joined_log_string(plyr);

								if (g_player_service->get_self()->is_host())
								{
									LOG(WARNING) << str;
									dynamic_cast<player_command*>(command::get(RAGE_JOAAT("hostkick")))->call(plyr);
									return;
								}

								if (!plyr->is_spammer)
									g_notification_service->push_warning("Carefull", str, true);

								return;
							}
							else
							{
								auto is_friend = friends_service::is_friend(rockstar_id);

								if (is_friend && !plyr->is_known_player)
									known_player_nm::toggle(plyr, true);

								if (g_session.lock_session && g_player_service->get_self()->is_host() && !is_friend)
								{
									dynamic_cast<player_command*>(command::get(RAGE_JOAAT("hostkick")))->call(plyr);
									g_notification_service->push_warning("Lock Session", std::format("Player {} denied entry.", player_name), true);
									return;
								}
							}
						}

						if (is_spoofed_host_token(host_token))
							g_reactions.spoofed_host_token.process(plyr, false, Infraction::SPOOFED_HOST_TOKEN, true);
					}
				});
		}

		return result;
	}
}
