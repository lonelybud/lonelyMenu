#include "backend/player_command.hpp"
#include "core/data/reactions.hpp"
#include "core/data/session.hpp"
#include "core/scr_globals.hpp"
#include "fiber_pool.hpp"
#include "hooking/hooking.hpp"
#include "services/bad_players/bad_players.hpp"
#include "services/friends/friends_service.hpp"
#include "services/known_players/known_players.hpp"
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
		auto host_token  = net_player_data->m_host_token;
		bool is_host     = player->is_host();

		if (new_index == static_cast<uint8_t>(-1))
		{
			g_player_service->player_leave(player);
			g_session.next_host_list.delete_plyr(player->m_player_id);

			LOGF(INFO, "Player left '{}', slot #{}. RID: {}", net_player_data->m_name, (int)player->m_player_id, rockstar_id);

			return g_hooking->get_original<hooks::assign_physical_index>()(netPlayerMgr, player, new_index);
		}

		const auto result = g_hooking->get_original<hooks::assign_physical_index>()(netPlayerMgr, player, new_index);

		auto plyr = (player == g_player_service->get_self()->get_net_game_player()) ? nullptr : g_player_service->player_join(player, host_token);

		if (!plyr || !is_host)
			g_session.next_host_list.insert_plyr(player->m_player_id, host_token, net_player_data->m_name);

		if (plyr)
			g_fiber_pool->queue_job([plyr, rockstar_id, host_token, is_host] {
				if (plyr && plyr->is_valid())
				{
					auto is_blocked  = g_bad_players_service.is_blocked(rockstar_id);
					auto is_known    = g_known_players_service.is_known(rockstar_id);
					auto is_friend   = plyr->is_friend();
					auto player_name = plyr->get_name();
					auto id          = plyr->id();

					auto join_str = std::format("'{}'{}, slot #{}, RID: {}", player_name, is_host ? "(host)" : "", id, rockstar_id);

					if (is_blocked)
					{
						plyr->is_blocked   = true;
						plyr->is_spammer   = g_bad_players_service.bad_players_list[rockstar_id].s;
						plyr->spam_message = g_bad_players_service.bad_players_list[rockstar_id].m;

						if (!plyr->is_spammer)
						{
							g_notification_service.push_warning("Blocked Player Joined", join_str, true);
							// if (SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH("maintransition"_J) == 0)
							// 	dynamic_cast<player_command*>(command::get("desync"_J))->call(plyr);
						}
					}
					else if (is_friend)
					{
						g_notification_service.push_success("Friend Player joined", join_str, true);
						if (!plyr->is_known_player)
						{
							plyr->is_known_player = true;
							g_known_players_service.add(plyr);
						}
					}
					else if (is_known)
					{
						plyr->is_known_player = true;
						g_notification_service.push_warning("Known Player Joined", join_str, true);
					}
					else
						LOG(INFO) << "Player joined : " << join_str;

					if (is_spoofed_host_token(host_token))
					{
						g_reactions.spoofed_host_token.process(plyr);

						if (g_player_service->get_self()->is_host() && !is_friend)
							dynamic_cast<player_command*>(command::get("hostkick"_J))->call(plyr);
					}
				}
			});

		return result;
	}
}
