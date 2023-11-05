#include "backend/player_command.hpp"
#include "core/scr_globals.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "services/notifications/notification_service.hpp"
#include "util/scripts.hpp"

namespace big
{
	class script_host_kick : player_command
	{
		using player_command::player_command;

		virtual CommandAccessLevel get_access_level() override
		{
			return CommandAccessLevel::TOXIC;
		}

		virtual void execute(player_ptr player, const command_arguments& _args, const std::shared_ptr<command_context> ctx) override
		{
			if (!player)
				return;

			if (!scripts::force_host(RAGE_JOAAT("freemode")))
			{
				const char* str = "Force script host failed!";
				LOG(WARNING) << str;
				g_notification_service->push_error("Script Host Kick", str);
				return;
			}

			LOG(VERBOSE) << "Script Host Kick > " << player->get_name();

			*scr_globals::gsbd_kicking.at(player->id(), 1).as<bool*>() = true;
		}
	};

	script_host_kick g_script_host_kick("shkick", "Script Host Kick", "Blocked by most menus", 0, false);
}