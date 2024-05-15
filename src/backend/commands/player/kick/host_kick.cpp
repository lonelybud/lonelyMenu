#include "backend/player_command.hpp"
#include "natives.hpp"
#include "services/notifications/notification_service.hpp"

namespace big
{
	class host_kick : player_command
	{
		using player_command::player_command;

		virtual void execute(player_ptr player) override
		{
			if (!g_player_service->get_self()->is_host())
			{
				g_notification_service.push_error("Host kick", std::format("You need to be host to kick {}", player->m_name), true);
				return;
			}

			if (player && player->is_valid())
			{
				g_notification_service.push_success("Kick", std::format("Host kick to {}", player->m_name), true);
				NETWORK::NETWORK_SESSION_KICK_PLAYER(player->id());
			}
		}
	};

	host_kick g_host_kick("hostkick", "Host Kick", "Host kick that only works when host");
}
