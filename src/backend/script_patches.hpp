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
		g_script_patcher_service->add_patch({RAGE_JOAAT("freemode"),
		    "freemode1",
		    "2D 01 08 00 ? 38 00 5D ? ? ? 2A 06",
		    5,
		    {0x71, 0x2E, 0x01, 0x01},
		    &g_session.decloak_players});
		g_script_patcher_service->add_patch({RAGE_JOAAT("freemode"), "script host kick", "2D 01 04 00 ? 2C ? ? ? 5D ? ? ? 71 57 ? ? 2C", 5, {0x2E, 0x01, 0x00}, nullptr}); // script host kick
		g_script_patcher_service->add_patch({RAGE_JOAAT("freemode"), "end session kick protection", "5D ? ? ? 76 57 ? ? 5D ? ? ? 76", 0, {0x2E, 0x00, 0x00}, nullptr}); // end session kick protection
		g_script_patcher_service->add_patch({RAGE_JOAAT("freemode"),
		    "disable population load balancing",
		    "2D 00 07 00 00 7B",
		    5,
		    {0x2E, 0x00, 0x00},
		    &g_debug.disable_population_load_balancing}); // disable population load balancing
		g_script_patcher_service->add_patch({RAGE_JOAAT("shop_controller"), "despawn bypass", "2D 01 04 00 00 2C ? ? ? 56 ? ? 71", 5, {0x71, 0x2E, 0x01, 0x01}, nullptr}); // despawn bypass
		g_script_patcher_service->add_patch({RAGE_JOAAT("shop_controller"), "godmode/invisibility detection bypass", "2D 01 03 00 00 5D ? ? ? 06 56 ? ? 2E ? ? 2C", 5, {0x2E, 0x01, 0x00}, nullptr}); // godmode/invisibility detection bypass

		LOG(INFO) << "Script patches: registered";

		for (auto& entry : *g_pointers->m_gta.m_script_program_table)
		{
			if (entry.m_program)
				g_script_patcher_service->on_script_load(entry.m_program, "m_script_program_table");
		}
	}
}