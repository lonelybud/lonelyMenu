#include "hooking/hooking.hpp"
#include "util/chat.hpp"

namespace big
{
	bool hooks::send_chat_message(void* team_mgr, rage::rlGamerInfo* local_gamer_info, char* message, bool is_team)
	{
		chat::send_message(message, nullptr, is_team, false);
		return true;
	}
}