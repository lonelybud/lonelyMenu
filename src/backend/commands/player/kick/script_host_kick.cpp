#include "backend/player_command.hpp"
#include "core/scr_globals.hpp"
#include "services/notifications/notification_service.hpp"
#include "util/scripts.hpp"

namespace big
{
	class script_host_kick : player_command
	{
		using player_command::player_command;

		virtual void execute(player_ptr player) override
		{
			if (player && player->is_valid())
			{
				if (!scripts::force_migration("freemode"_J, true))
				{
					g_notification_service.push_error("Script Host Kick",
					    std::format("You need to be freemode host to kick {}", player->m_name),
					    true);
					return;
				}

				g_notification_service.push_success("Kick", std::format("Script Host Kick to {}", player->m_name), true);

				*scr_globals::gsbd_kicking.at(player->id(), 1).as<bool*>() = true;
			}
		}
	};

	script_host_kick g_script_host_kick("shkick", "Script Host Kick", "Requires host protections off");
}