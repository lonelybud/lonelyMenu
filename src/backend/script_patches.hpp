#pragma once
#include "pointers.hpp"
#include "services/script_patcher/script_patcher_service.hpp"

#include <script/scrProgramTable.hpp>

namespace big
{
	void register_script_patches()
	{
		g_script_patcher_service->add_patch({RAGE_JOAAT("freemode"), "2D 01 04 00 ? 2C ? ? ? 5D ? ? ? 71 57 ? ? 2C", 5, {0x2E, 0x01, 0x00}, nullptr}); // script host kick
		g_script_patcher_service->add_patch({RAGE_JOAAT("freemode"), "5D ? ? ? 76 57 ? ? 5D ? ? ? 76", 0, {0x2E, 0x00, 0x00}, nullptr}); // end session kick protection
		g_script_patcher_service->add_patch({RAGE_JOAAT("freemode"), "2D 00 07 00 00 7B", 5, {0x2E, 0x00, 0x00}, nullptr}); // disable population load balancing
		g_script_patcher_service->add_patch({RAGE_JOAAT("shop_controller"), "2D 01 03 00 00 5D ? ? ? 06 56 ? ? 2E ? ? 2C", 5, {0x2E, 0x01, 0x00}, nullptr}); // godmode/invisibility detection bypass

		for (auto& entry : *g_pointers->m_gta.m_script_program_table)
		{
			if (entry.m_program)
				g_script_patcher_service->on_script_load(entry.m_program);
		}
	}
}