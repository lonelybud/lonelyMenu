#include "backend/player_command.hpp"
#include "core/data/infractions.hpp"
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
				g_notification_service->push_error("Host kick", std::format("Host kick failed on {}", player->get_name()), true);
				return;
			}

			if (!player->is_blocked
			    && (player->infractions.contains((int)Infraction::TRIED_KICK_PLAYER) || player->infractions.contains((int)Infraction::TRIED_CRASH_PLAYER)))
				bad_players_nm::add_player(player, true, player->is_spammer);

			g_notification_service->push_success("Kick", std::format("Host kick to {}", player->get_name()), true);
			NETWORK::NETWORK_SESSION_KICK_PLAYER(player->id());
		}
	};

	host_kick g_host_kick("hostkick", "Host Kick", "Host kick that only works when host");
}