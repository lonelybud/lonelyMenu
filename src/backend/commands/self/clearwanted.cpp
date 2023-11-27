#include "backend/command.hpp"
#include "natives.hpp"

namespace big
{
	class clear_wanted : command
	{
		using command::command;

		virtual void execute() override
		{
			if(g_local_player && g_local_player !=nullptr)
			{
				g_local_player->m_player_info->m_wanted_level = 0;
				g_local_player->m_player_info->m_is_wanted = false;
			}
		}
	};

	clear_wanted g_clear_wanted("clearwantedlvl", "Clear Wanted Level", "Clears your wanted level");
}