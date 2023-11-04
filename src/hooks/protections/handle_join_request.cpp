#include "hooking.hpp"
#include "pointers.hpp"
#include "services/notifications/notification_service.hpp"
#include "services/recent_modders/recent_modders.hpp"

#include <network/CJoinRequestContext.hpp>
#include <network/CMsgJoinResponse.hpp>
#include <network/snSession.hpp>

namespace big
{
	bool hooks::handle_join_request(Network* network, rage::snSession* session, rage::rlGamerInfo* player_info, CJoinRequestContext* ctx, BOOL is_transition_session)
	{
		if (recent_modders_nm::is_blocked(player_info->m_gamer_handle.m_rockstar_id))
		{
			CMsgJoinResponse response{};
			response.m_status_code = 21;
			g_pointers->m_gta.m_write_join_response_data(&response, ctx->m_join_response_data, 512, &ctx->m_join_response_size);
			g_notification_service->push_success("Join Blocked", std::format("Request denied to player {}", player_info->m_name));
			LOG(WARNING) << "A blocked player " << player_info->m_name << " requested to join. Denied!";
			return false;
		}
		else
			return g_hooking->get_original<hooks::handle_join_request>()(network, session, player_info, ctx, is_transition_session);
	}
}