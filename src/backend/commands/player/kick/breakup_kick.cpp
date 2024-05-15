#include "backend/player_command.hpp"
#include "gta_util.hpp"
#include "pointers.hpp"
#include "services/notifications/notification_service.hpp"

namespace big
{
	class breakup_kick : player_command
	{
		using player_command::player_command;

		virtual void execute(player_ptr player) override
		{
			if (!g_player_service->get_self()->is_host())
			{
				g_notification_service.push_error("Breakup Kick", std::format("You need to be host to kick {}", player->m_name), true);
				return;
			}

			if (player && player->is_valid())
			{
				rage::snMsgRemoveGamersFromSessionCmd cmd{};
				cmd.m_session_id = gta_util::get_network()->m_game_session_ptr->m_rline_session.m_session_id;
				cmd.m_num_peers  = 1;
				cmd.m_handles[0] = player->get_net_data()->m_gamer_handle;

				g_notification_service.push_success("Kick", std::format("Breakup Kick to {}", player->m_name), true);

				g_pointers->m_gta.m_handle_remove_gamer_cmd(gta_util::get_network()->m_game_session_ptr, player->get_session_player(), &cmd);
			}
		}
	};

	breakup_kick g_breakup_kick("breakup", "Breakup Kick", "");
}