#include "backend.hpp"

#include "looped/looped.hpp"
#include "looped_command.hpp"
#include "script.hpp"
#include "script_patches.hpp"
#include "services/bad_players/bad_players.hpp"
#include "thread_pool.hpp"
#include "util/globals.hpp"

namespace big
{
	void backend::loop()
	{
		for (auto& command : g_bool_commands)
			command->refresh();

		register_script_patches();

		bad_players_nm::load_blocked_list();

		while (g_running)
		{
			looped::system_self_globals();
			looped::system_update_pointers();
			looped::system_desync_kick_protection();
			looped::system_spoofing();

			for (auto command : g_looped_commands)
				if (*g_pointers->m_gta.m_is_session_started && (globals::get_interior_from_player(self::id) == 0))
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
			looped::hud_transition_state();

			script::get_current()->yield();
		}
	}
}
