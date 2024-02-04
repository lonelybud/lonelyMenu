#include "backend.hpp"

#include "looped/looped.hpp"
#include "looped_command.hpp"
#include "script.hpp"
#include "script_patches.hpp"
#include "services/bad_players/bad_players.hpp"
#include "services/context_menu/context_menu_service.hpp"
#include "services/custom_teleport/custom_teleport_service.hpp"
#include "services/known_players.hpp"
#include "services/mobile/mobile_service.hpp"
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
			looped::host_token_spoofing();

			for (auto command : g_looped_commands)
				if (command->is_enabled())
					command->on_tick();

			script::get_current()->yield();
		}
	}

	void backend::misc_loop()
	{
		LOG(INFO) << "Starting script: Miscellaneous";

		g_tunables_service->run_script(); // blocking until tuneables are loaded
		looped::custom_thread();          // non-blocking

		while (g_running)
		{
			looped::hud_disable_input();
			looped::vehicle_auto_drive();
			looped::self_host_notify();
			looped::death_check();

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
}
