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

			script::get_current()->yield();
		}
	}

	void backend::players_state()
	{
		LOG(INFO) << "Starting script: Players State";

		while (g_running)
		{
			if (g_player_service->get_self()->is_host())
			{
				if (!g_session.notified_as_host)
				{
					g_session.notified_as_host = true;
					g_notification_service->push_success("You are host", "", true);
				}

				for (auto& [_, player] : g_player_service->players())
					if (!player->has_joined && scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[player->id()].FreemodeState == eFreemodeState::RUNNING)
						player->has_joined = true;
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
