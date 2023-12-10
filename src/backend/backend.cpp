#include "backend.hpp"

#include "core/data/debug.hpp"
#include "looped/looped.hpp"
#include "looped_command.hpp"
#include "script.hpp"
#include "script_patches.hpp"
#include "services/bad_players/bad_players.hpp"
#include "services/tunables/tunables_service.hpp"
#include "thread_pool.hpp"
#include "util/globals.hpp"
#include "util/lua_script.cpp"

namespace big
{
	void backend::loop()
	{
		register_script_patches();

		bad_players_nm::load_blocked_list();

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

			if (g_debug.fm_mission_controller_cart_grab)
			{
				constexpr int fm_mission_controller_cart_grab       = 10247;
				constexpr int fm_mission_controller_cart_grab_speed = 14;

				if (lua_script::locals::get_int("fm_mission_controller", fm_mission_controller_cart_grab) == 3)
					lua_script::locals::set_int("fm_mission_controller", fm_mission_controller_cart_grab, 4);
				else if (lua_script::locals::get_int("fm_mission_controller", fm_mission_controller_cart_grab) == 4)
					lua_script::locals::set_float("fm_mission_controller", fm_mission_controller_cart_grab + fm_mission_controller_cart_grab_speed, 2);
			}

			script::get_current()->yield();
		}
	}

	void backend::tunables_script()
	{
		g_tunables_service->run_script();
	}
}
