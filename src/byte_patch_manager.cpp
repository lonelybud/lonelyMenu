#include "byte_patch_manager.hpp"

#include "gta/net_array.hpp"
#include "hooking/hooking.hpp"
#include "memory/byte_patch.hpp"
#include "pointers.hpp"
#include "util/vehicle.hpp"

namespace big
{
	static void init()
	{
		// Skip matchmaking session validity checks
		memory::byte_patch::make(g_pointers->m_gta.m_is_matchmaking_session_valid.as<void*>(), std::to_array({0xB0, 0x01, 0xC3}))
		    ->apply(); // has no observable side effects

		// Disable cheat activated netevent when creator warping
		memory::byte_patch::make(g_pointers->m_gta.m_creator_warp_cheat_triggered_patch.as<uint8_t*>(), 0xEB)->apply();

		// Crash Trigger
		memory::byte_patch::make(g_pointers->m_gta.m_crash_trigger.add(4).as<uint8_t*>(), 0x00)->apply();

		// Script VM patches

		memory::byte_patch::make(g_pointers->m_gta.m_script_vm_patch_1.add(2).as<uint32_t*>(), 0xc9310272)->apply();
		memory::byte_patch::make(g_pointers->m_gta.m_script_vm_patch_1.add(6).as<uint16_t*>(), 0x9090)->apply();

		memory::byte_patch::make(g_pointers->m_gta.m_script_vm_patch_2.add(2).as<uint32_t*>(), 0xc9310272)->apply();
		memory::byte_patch::make(g_pointers->m_gta.m_script_vm_patch_2.add(6).as<uint16_t*>(), 0x9090)->apply();

		memory::byte_patch::make(g_pointers->m_gta.m_script_vm_patch_3.add(2).as<uint32_t*>(), 0xd2310272)->apply();
		memory::byte_patch::make(g_pointers->m_gta.m_script_vm_patch_3.add(6).as<uint16_t*>(), 0x9090)->apply();

		memory::byte_patch::make(g_pointers->m_gta.m_script_vm_patch_4.add(2).as<uint32_t*>(), 0xd2310272)->apply();
		memory::byte_patch::make(g_pointers->m_gta.m_script_vm_patch_4.add(6).as<uint16_t*>(), 0x9090)->apply();

		memory::byte_patch::make(g_pointers->m_gta.m_script_vm_patch_5.add(2).as<uint32_t*>(), 0xd2310272)->apply();
		memory::byte_patch::make(g_pointers->m_gta.m_script_vm_patch_5.add(6).as<uint16_t*>(), 0x9090)->apply();

		memory::byte_patch::make(g_pointers->m_gta.m_script_vm_patch_6.add(2).as<uint32_t*>(), 0xd2310272)->apply();
		memory::byte_patch::make(g_pointers->m_gta.m_script_vm_patch_6.add(6).as<uint16_t*>(), 0x9090)->apply();

		// Patch script network check
		memory::byte_patch::make(g_pointers->m_gta.m_model_spawn_bypass, std::vector{0x90, 0x90})->apply(); // this is no longer integrity checked

		// Prevent the game from crashing when flooded with outgoing events
		memory::byte_patch::make(g_pointers->m_gta.m_free_event_error, std::vector{0x90, 0x90, 0x90, 0x90, 0x90})->apply();

		// Always send the special ability event
		memory::byte_patch::make(g_pointers->m_gta.m_activate_special_ability_patch, std::to_array({0xB0, 0x01, 0xC3}))->apply();
	}

	byte_patch_manager::byte_patch_manager()
	{
		init();

		g_byte_patch_manager = this;
	}

	byte_patch_manager::~byte_patch_manager()
	{
		memory::byte_patch::restore_all();

		g_byte_patch_manager = nullptr;
	}
}