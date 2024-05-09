#include "backend/player_command.hpp"
#include "natives.hpp"
#include "services/bad_players/bad_players.hpp"
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
				g_notification_service.push_error("Host kick", std::format("Host kick failed on {}", player->m_name), true);
				return;
			}

			if (player && player->is_valid())
			{
				NETWORK::NETWORK_SESSION_KICK_PLAYER(player->id());
				g_notification_service.push_success("Kick", std::format("Host kick to {}", player->m_name), true);
			}
		}
	};

	host_kick g_host_kick("hostkick", "Host Kick", "Host kick that only works when host");
}
