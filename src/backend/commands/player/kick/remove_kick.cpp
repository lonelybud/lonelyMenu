#include "backend/player_command.hpp"
#include "gta_util.hpp"
#include "services/notifications/notification_service.hpp"

namespace big
{
	class remove_kick : player_command
	{
		using player_command::player_command;

		virtual void execute(player_ptr player) override
		{
			if (!g_player_service->get_self()->is_host())
			{
				g_notification_service.push_error("Remove Kick", std::format("You need to be host to kick {}", player->m_name), true);
				return;
			}

			if (player && player->is_valid())
			{
				gta_util::get_network_player_mgr()->RemovePlayer(player->get_net_game_player());
				g_notification_service.push_success("Kick", std::format("Remove kick to {}", player->m_name), true);
			}
		}
	};

	remove_kick g_remove_kick("removekick", "Remove Kick", "Remove Player from network_player_mgr");
}