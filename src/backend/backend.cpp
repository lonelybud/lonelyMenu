#include "backend.hpp"

#include "looped/looped.hpp"
#include "looped_command.hpp"
#include "script.hpp"
#include "script_patches.hpp"
#include "services/context_menu/context_menu_service.hpp"
#include "services/tunables/tunables_service.hpp"
#include "util/session.hpp"

namespace big
{
	void backend::loop()
	{
		register_script_patches();

		while (g_running)
		{
			looped::update_globals();
			looped::system_update_desync_kick();

			for (auto command : g_looped_commands)
				if (command->is_enabled())
					command->on_tick();

			script::get_current()->yield();
		}
	}

	void backend::misc_loop()
	{
		session::set_host_tokens();
		g_tunables_service->run_script(); // blocking until tuneables are loaded
		looped::custom_thread();          // non-blocking

		while (g_running)
		{
			looped::hud_transition_state();
			looped::hud_disable_input();
			looped::vehicle_auto_drive();
			looped::death_check();

			script::get_current()->yield();
		}
	}

	void backend::disable_control_action_loop()
	{
		while (g_running)
		{
			context_menu_service::disable_control_action_loop();

			script::get_current()->yield();
		}
	}

	void backend::ptfx_loop()
	{
		while (g_running)
		{
			looped::ptfx_loop();
			script::get_current()->yield();
		}
	}
}
