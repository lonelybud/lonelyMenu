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
			if (!scripts::force_migration(RAGE_JOAAT("freemode")))
			{
				g_notification_service->push_error("Script Host kick",
				    std::format("Script Host kick failed on {}", player->get_name()),
				    true);
				return;
			}

			g_notification_service->push_success("Kick", std::format("Script Host Kick to {}", player->get_name()), true);

			*scr_globals::gsbd_kicking.at(player->id(), 1).as<bool*>() = true;
		}
	};

	script_host_kick g_script_host_kick("shkick", "Script Host Kick", "Blocked by most menus");
}