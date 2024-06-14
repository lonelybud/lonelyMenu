#include "core/data/session.hpp"
#include "hooking/hooking.hpp"
#include "services/players/player_service.hpp"
#include "util/chat.hpp"

namespace big
{
	bool hooks::send_chat_message(void* team_mgr, rage::rlGamerInfo* local_gamer_info, char* message, bool is_team)
	{
		if (g_session.log_chat)
			chat::log_chat_to_disk(message, g_player_service->get_self()->m_name);

		return g_hooking->get_original<hooks::send_chat_message>()(team_mgr, local_gamer_info, message, is_team);
	}
}