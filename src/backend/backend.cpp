#include "backend.hpp"

#include "core/data/misc.hpp"
#include "core/data/session.hpp"
#include "core/scr_globals.hpp"
#include "gta/joaat.hpp"
#include "looped/looped.hpp"
#include "looped_command.hpp"
#include "script.hpp"
#include "script_patches.hpp"
#include "services/bad_players/bad_players.hpp"
#include "services/context_menu/context_menu_service.hpp"
#include "services/custom_teleport/custom_teleport_service.hpp"
#include "services/known_players.hpp"
#include "services/tunables/tunables_service.hpp"

namespace big
{
	void backend::loop()
	{
		register_script_patches();

		bad_players_nm::load_blocked_list();
		known_player_nm::load_list();
		g_custom_teleport_service.fetch_saved_locations();

		while (g_running)
		{
			looped::update_globals();
			looped::system_desync_kick_protection();
			looped::system_spoofing();

			for (auto command : g_looped_commands)
				if (command->is_enabled())
					command->on_tick();

			script::get_current()->yield();
		}
	}

	void backend::misc_loop()
	{
		LOG(INFO) << "Starting script: Miscellaneous";

		while (g_running)
		{
			looped::hud_disable_input();

			if (g_misc.disable_clothing_validation)
			{
				*scr_globals::reset_clothing.as<PBOOL>() = FALSE;
				if (auto tunable = g_tunables_service->get_tunable<PBOOL>(RAGE_JOAAT("DISABLE_CLOTHING_SAVE_SLOT_VALIDATION")))
					*tunable = TRUE;
			}

			if (g_session.next_host_list.determine_new_host)
			{
				if (g_player_service->get_self()->is_host())
					g_session.next_host_list.current_host = g_player_service->get_self();
				else
					for (const auto& [_, plyr] : g_player_service->players())
						if (plyr->is_host())
						{
							g_session.next_host_list.current_host = plyr;
							break;
						}

				if (g_session.next_host_list.current_host)
				{
					g_session.next_host_list.determine_new_host = false;
					g_session.next_host_list.delete_plyr(g_session.next_host_list.current_host->id()); // filter out new host from the list
					g_notification_service->push_success("Host Determined",
					    std::format("'{}' is the new host", g_session.next_host_list.current_host->get_name()),
					    true);
				}
			}

			script::get_current()->yield();
		}
	}

	void backend::disable_control_action_loop()
	{
		LOG(INFO) << "Starting script: Disable Control Action";

		while (g_running)
		{
			context_menu_service::disable_control_action_loop();

			script::get_current()->yield();
		}
	}

	void backend::tunables_script()
	{
		g_tunables_service->run_script();
	}
}
