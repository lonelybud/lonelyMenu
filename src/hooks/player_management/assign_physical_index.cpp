#include "backend/player_command.hpp"
#include "core/data/admin_rids.hpp"
#include "core/data/reactions.hpp"
#include "core/data/recent_spoofed_host_tokens.hpp"
#include "core/data/session.hpp"
#include "core/scr_globals.hpp"
#include "fiber_pool.hpp"
#include "hooking/hooking.hpp"
#include "services/bad_players/bad_players.hpp"
#include "services/friends/friends_service.hpp"
#include "services/known_players/known_players.hpp"
#include "services/players/player_service.hpp"
#include "util/player.hpp"
#include "util/session.hpp"

#include <script/globals/GlobalPlayerBD.hpp>

namespace big
{
	void* hooks::assign_physical_index(CNetworkPlayerMgr* netPlayerMgr, CNetGamePlayer* player, uint8_t new_index)
	{
		const auto* net_player_data = player->get_net_data();

		auto rockstar_id = net_player_data->m_gamer_handle.m_rockstar_id;
		auto host_token  = net_player_data->m_host_token;

		if (new_index == static_cast<uint8_t>(-1))
		{
			g_player_service->player_leave(player, rockstar_id);
			g_session.next_host_list.delete_plyr(player->m_player_id);

			LOGF(INFO, "Player left '{}', slot #{}. RID: {}", net_player_data->m_name, (int)player->m_player_id, rockstar_id);

			return g_hooking->get_original<hooks::assign_physical_index>()(netPlayerMgr, player, new_index);
		}

		const auto result = g_hooking->get_original<hooks::assign_physical_index>()(netPlayerMgr, player, new_index);
		auto id           = player->m_player_id;

		if (player == g_player_service->get_self()->get_net_game_player())
		{
			g_session.next_host_list.insert_plyr(id, host_token, net_player_data->m_name);
			LOG(INFO) << "You joined the session.";
		}
		else
		{
			g_player_service->player_join(player, host_token);
			g_fiber_pool->queue_job([id, rockstar_id, host_token] {
				if (auto plyr = g_player_service->get_by_id(id); plyr && plyr->is_valid())
				{
					auto is_blocked  = g_bad_players_service.is_blocked(rockstar_id);
					auto is_known    = g_known_players_service.is_known(rockstar_id);
					auto is_friend   = plyr->is_friend();
					auto player_name = plyr->m_name;
					auto id          = plyr->id();
					auto is_host     = plyr->is_host();
					bool kick        = false;
					bool imhost      = g_player_service->get_self()->is_host();

					if (!plyr->is_host())
						g_session.next_host_list.insert_plyr(id, host_token, player_name);

					auto join_str = std::format("'{}'{}, slot #{}, RID: {}, tk: {}", player_name, is_host ? "(host)" : "", id, rockstar_id, host_token);

					if (is_known)
						plyr->is_known_player = true;

					if (is_blocked)
					{
						plyr->is_blocked   = true;
						plyr->is_spammer   = g_bad_players_service.bad_players_list[rockstar_id].s;
						plyr->spam_message = g_bad_players_service.bad_players_list[rockstar_id].m;

						if (plyr->is_spammer)
							LOG(INFO) << "Spammer joined: " << join_str;
						else
						{
							g_notification_service.push_warning("Blocked Player Joined", join_str, true);
							kick = imhost || g_session.block_blocked_players_join;
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
						g_notification_service.push_warning("Known Player Joined", join_str, true);
					else
						LOG(INFO) << "Player joined: " << join_str;

					if (admin_rids.contains(rockstar_id))
						g_reactions.rockstar_admin.process(plyr);

					if (session::is_spoofed_host_token(host_token))
					{
						g_recent_spoofed_host_tokens[rockstar_id] = player_name;
						g_reactions.spoofed_host_token.process(plyr);
						kick = imhost || g_session.block_spoofed_tokens_join;
					}

					if (!is_friend && kick && SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH("maintransition"_J) == 0)
					{
						if (imhost)
							dynamic_cast<player_command*>(command::get("breakup"_J))->call(plyr);
						else
							dynamic_cast<player_command*>(command::get("shkick"_J))->call(plyr);
					}

					if (plyr->get_net_data()->m_nat_type == 0)
						g_reactions.desync_protection.process(plyr);
				}
			});
		}

		return result;
	}
}