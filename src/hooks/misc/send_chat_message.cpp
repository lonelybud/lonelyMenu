#include "core/data/debug.hpp"
#include "core/data/session.hpp"
#include "hooking/hooking.hpp"
#include "services/custom_chat_buffer.hpp"
#include "services/players/player_service.hpp"
#include "util/chat.hpp"

namespace big
{
	bool hooks::send_chat_message(void* team_mgr, rage::rlGamerInfo* local_gamer_info, char* message, bool is_team)
	{
		// if (g_debug.use_default_chat)
		// {
			if (g_session.log_chat_messages_to_textbox)
				g_custom_chat_buffer.append_msg(g_player_service->get_self()->get_name(), message);

			return g_hooking->get_original<hooks::send_chat_message>()(team_mgr, local_gamer_info, message, is_team);
		// }

		// chat::send_message(message, nullptr, is_team, false);
		// return true;
	}
}