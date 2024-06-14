#include "core/data/recent_spoofed_host_tokens.hpp"
#include "core/data/session.hpp"
#include "hooking/hooking.hpp"
#include "pointers.hpp"
#include "services/blocked_players/blocked_players.hpp"
#include "services/friends/friends_service.hpp"
#include "services/notifications/notification_service.hpp"
#include "util/session.hpp"

#include <network/CJoinRequestContext.hpp>
#include <network/CMsgJoinResponse.hpp>
#include <network/snSession.hpp>

namespace big
{
	bool hooks::handle_join_request(Network* network, rage::snSession* session, rage::rlGamerInfo* player_info, CJoinRequestContext* ctx, BOOL is_transition_session)
	{
		auto rockstar_id = player_info->m_gamer_handle.m_rockstar_id;
		auto is_friend   = friends_service::is_friend(rockstar_id);
		auto is_blocked  = g_blocked_players_service.is_blocked(rockstar_id);

		auto block_join        = (g_session.block_joins && !is_friend) || (g_session.block_friend_joins && is_friend);
		auto has_spoofed_token = !is_friend && session::is_spoofed_host_token(player_info->m_host_token);

		if (block_join || is_blocked || has_spoofed_token)
		{
			auto str = std::format("Join Request denied to Player: {} ({})", player_info->m_name, rockstar_id);

			CMsgJoinResponse response{};
			response.m_status_code = 1;
			g_pointers->m_gta.m_write_join_response_data(&response, ctx->m_join_response_data, 512, &ctx->m_join_response_size);

			if (is_blocked)
			{
				if (!g_blocked_players_service.blocked_players_list[rockstar_id].s)
					g_notification_service.push_success("Join Blocked", str, true);
			}
			else if (has_spoofed_token)
			{
				LOG(WARNING) << std::format("Join Request denied for spoofed host: {} ({})", player_info->m_name, rockstar_id);
				g_recent_spoofed_host_tokens[rockstar_id] = player_info->m_name;
			}
			else
				LOG(WARNING) << str;

			return false;
		}
		else
			return g_hooking->get_original<hooks::handle_join_request>()(network, session, player_info, ctx, is_transition_session);
	}
}
