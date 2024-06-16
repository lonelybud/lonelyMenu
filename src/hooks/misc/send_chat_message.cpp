#include "core/data/session.hpp"
#include "hooking/hooking.hpp"
#include "services/notifications/notification_service.hpp"
#include "services/players/player_service.hpp"
#include "util/chat.hpp"

namespace big
{
	bool hooks::send_chat_message(void* team_mgr, rage::rlGamerInfo* local_gamer_info, char* message, bool is_team)
	{
		if (g_session.sending_chat_msg)
		{
			g_notification_service.push_error("Send Message Failed", "Already in progress!");
			return true;
		}

		g_session.sending_chat_msg = true;

		auto val = g_hooking->get_original<hooks::send_chat_message>()(team_mgr, local_gamer_info, message, is_team);

		if (g_session.log_chat)
			chat::log_chat_to_disk(message, g_player_service->get_self()->m_name);

		g_player_service->get_self()->last_msg_time = std::chrono::system_clock::now();
		g_session.sending_chat_msg                  = false;

		return val;
	}
}