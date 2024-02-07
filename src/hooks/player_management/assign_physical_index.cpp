#include "backend/player_command.hpp"
#include "core/data/reactions.hpp"
#include "core/data/session.hpp"
#include "core/scr_globals.hpp"
#include "fiber_pool.hpp"
#include "hooking/hooking.hpp"
#include "services/bad_players/bad_players.hpp"
#include "services/friends/friends_service.hpp"
#include "services/known_players.hpp"
#include "services/players/player_service.hpp"
#include "util/player.hpp"

#include <script/globals/GlobalPlayerBD.hpp>

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
		auto is_dev_qa   = player->m_is_rockstar_dev || player->m_is_rockstar_qa;

		if (new_index == static_cast<uint8_t>(-1))
		{
			g_player_service->player_leave(player);
			g_session.next_host_list.delete_plyr(player->m_player_id);

			LOGF(INFO, "Player left '{}', slot #{}. RID: {}", player_name, (int)player->m_player_id, rockstar_id);

			return g_hooking->get_original<hooks::assign_physical_index>()(netPlayerMgr, player, new_index);
		}

		const auto result = g_hooking->get_original<hooks::assign_physical_index>()(netPlayerMgr, player, new_index);

		auto plyr = (player == g_player_service->get_self()->get_net_game_player()) ? nullptr : g_player_service->player_join(player, host_token);

		if (!plyr || !plyr->is_host())
			g_session.next_host_list.insert_plyr(player->m_player_id, host_token, player_name);

		if (net_player_data)
		{
			LOGF(INFO,
			    "{} joined '{}'{}, slot #{}. RID: {}",
			    (plyr && plyr->is_friend() ? "*** Friend" : "Player"),
			    player_name,
			    player->is_host() ? "(host)" : "",
			    (int)player->m_player_id,
			    rockstar_id);

			if (plyr)
				g_fiber_pool->queue_job([plyr, rockstar_id, player_name, host_token, is_dev_qa] {
					if (plyr && plyr->is_valid())
					{
						if (bad_players_nm::is_blocked(rockstar_id))
						{
							auto str = get_blocked_player_joined_log_string(plyr);

							if (g_player_service->get_self()->is_host() && *g_pointers->m_gta.m_is_session_started)
							{
								LOG(WARNING) << str;
								dynamic_cast<player_command*>(command::get(RAGE_JOAAT("hostkick")))->call(plyr);
								return;
							}

							if (!plyr->is_spammer)
							{
								g_notification_service->push_warning("Carefull", str, true);

								// if (SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH(RAGE_JOAAT("maintransition")) == 0)
								// 	dynamic_cast<player_command*>(command::get(RAGE_JOAAT("desync")))->call(plyr);
							}
						}

						if (is_dev_qa)
							g_reactions.rockstar_dev.process(plyr);

						if (known_player_nm::is_known(rockstar_id))
						{
							plyr->is_known_player = true;
							if (!plyr->is_friend())
								g_notification_service->push_warning("Known Player joined", player_name);
						}

						if (plyr->is_friend())
						{
							g_notification_service->push_success("Friend joined", player_name);
							if (!plyr->is_known_player)
								known_player_nm::toggle(plyr, true);
						}

						if (is_spoofed_host_token(host_token))
							g_reactions.spoofed_host_token.process(plyr);
					}
				});
		}

		return result;
	}
}
