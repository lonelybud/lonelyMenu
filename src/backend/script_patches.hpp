#pragma once
#include "core/data/debug.hpp"
#include "core/data/session.hpp"
#include "gta/joaat.hpp"
#include "pointers.hpp"
#include "services/script_patcher/script_patcher_service.hpp"

#include <script/scrProgramTable.hpp>

namespace big
{
	void register_script_patches()
	{
		g_script_patcher_service->add_patch({"freemode"_J,
		    "decloak_players",
		    "2D 01 08 00 ? 38 00 5D ? ? ? 2A 06",
		    5,
		    {0x71, 0x2E, 0x01, 0x01},
		    &g_session.decloak_players});
		g_script_patcher_service->add_patch({"freemode"_J, "script host kick", "2D 01 04 00 ? 2C ? ? ? 5D ? ? ? 71 57 ? ? 2C", 5, {0x2E, 0x01, 0x00}, nullptr}); // script host kick
		g_script_patcher_service->add_patch({"freemode"_J, "end session kick protection", "5D ? ? ? 76 57 ? ? 5D ? ? ? 76", 0, {0x2E, 0x00, 0x00}, nullptr}); // end session kick protection
		g_script_patcher_service->add_patch({"shop_controller"_J, "despawn bypass", "2D 01 04 00 00 2C ? ? ? 56 ? ? 71", 5, {0x71, 0x2E, 0x01, 0x01}, nullptr}); // despawn bypass
		g_script_patcher_service->add_patch({"shop_controller"_J, "godmode/invisibility detection bypass", "2D 01 03 00 00 5D ? ? ? 06 56 ? ? 2E ? ? 2C", 5, {0x2E, 0x01, 0x00}, nullptr}); // godmode/invisibility detection bypass

		g_script_patcher_service->add_patch({"freemode"_J,
		    "unhide_players_from_player_list",
		    "2D 01 03 00 00 38 00 71 72 5D ? ? ? 06 56 ? ? 71 2E ? ? 2C ? ? ? 71",
		    5,
		    {0x72, 0x2E, 0x01, 0x01},
		    &g_session.unhide_players_from_player_list});

		LOG(INFO) << "Script patches: registered";

		for (auto& entry : *g_pointers->m_gta.m_script_program_table)
		{
			if (entry.m_program)
				g_script_patcher_service->on_script_load(entry.m_program, "m_script_program_table");
		}
	}
}
