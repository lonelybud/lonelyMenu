#include "native_hooks.hpp"

#include "all_scripts.hpp"
#include "carmod_shop.hpp"
#include "crossmap.hpp"
#include "gta_util.hpp"

#include <script/scrProgram.hpp>
#include <script/scrProgramTable.hpp>

namespace big
{
	static bool map_native(rage::scrNativeHash* hash)
	{
		for (auto const& mapping : g_crossmap)
		{
			if (mapping.first == *hash)
			{
				*hash = mapping.second;
				return true;
			}
		}

		return false;
	}

	native_hook::native_hook(rage::scrProgram* program, const std::unordered_map<rage::scrNativeHash, rage::scrNativeHandler>& native_replacements)
	{
		hook_instance(program, native_replacements);
	}

	native_hook::~native_hook()
	{
		if (m_handler_hook)
		{
			m_handler_hook->disable();
			m_handler_hook.reset();
		}

		if (m_vmt_hook)
		{
			m_vmt_hook->disable();
			m_vmt_hook.reset();
		}
	}

	void native_hook::hook_instance(rage::scrProgram* program, const std::unordered_map<rage::scrNativeHash, rage::scrNativeHandler>& native_replacements)
	{
		m_program  = program;
		m_vmt_hook = std::make_unique<vmt_hook>(m_program, 9);
		m_vmt_hook->hook(6, &scrprogram_dtor);
		m_vmt_hook->enable();

		m_handler_hook = std::make_unique<vmt_hook>(&m_program->m_native_entrypoints, m_program->m_native_count);
		m_handler_hook->enable();

		std::unordered_map<rage::scrNativeHandler, rage::scrNativeHandler> handler_replacements;

		for (auto& [replacement_hash, replacement_handler] : native_replacements)
		{
			auto native = replacement_hash;
			map_native(&native);

			auto og_handler = g_pointers->m_gta.m_get_native_handler(g_pointers->m_gta.m_native_registration_table, native);
			if (!og_handler)
				continue;

			handler_replacements[og_handler] = replacement_handler;
		}

		for (int i = 0; i < m_program->m_native_count; i++)
		{
			if (auto it = handler_replacements.find((rage::scrNativeHandler)program->m_native_entrypoints[i]);
			    it != handler_replacements.end())
			{
				m_handler_hook->hook(i, it->second);
			}
		}
	}

	void native_hook::scrprogram_dtor(rage::scrProgram* this_, char free_memory)
	{
		if (auto it = g_native_hooks->m_native_hooks.find(this_); it != g_native_hooks->m_native_hooks.end())
		{
			auto og_func = it->second->m_vmt_hook->get_original<decltype(&native_hook::scrprogram_dtor)>(6);
			it->second->m_vmt_hook->disable();
			it->second->m_vmt_hook.reset();
			it->second->m_handler_hook->disable();
			it->second->m_handler_hook.reset();
			g_native_hooks->m_native_hooks.erase(it);
			og_func(this_, free_memory);
		}
		else
		{
			LOG(FATAL) << "Cannot find hook for program";
		}
	}

	constexpr auto ALL_SCRIPT_HASH = RAGE_JOAAT("ALL_SCRIPTS");

	native_hooks::native_hooks()
	{
		add_native_detour(0x1CA59E306ECB80A5, all_scripts::NETWORK_SET_THIS_SCRIPT_IS_NETWORK_SCRIPT);
		add_native_detour(0xD1110739EEADB592, all_scripts::NETWORK_TRY_TO_SET_THIS_SCRIPT_IS_NETWORK_SCRIPT);
		add_native_detour(0x40EB1EFD921822BC, all_scripts::DO_NOTHING); // SECURITY::REGISTER_SCRIPT_VARIABLE
		add_native_detour(0x340A36A700E99699, all_scripts::DO_NOTHING); // SECURITY::UNREGISTER_SCRIPT_VARIABLE
		add_native_detour(0x8E580AB902917360, all_scripts::DO_NOTHING); // SECURITY::FORCE_CHECK_SCRIPT_VARIABLES

		add_native_detour(RAGE_JOAAT("shop_controller"), 0x34616828CD07F1A1, all_scripts::RETURN_FALSE); // prevent exploit reports

		for (auto& entry : *g_pointers->m_gta.m_script_program_table)
			if (entry.m_program)
				hook_program(entry.m_program);

		g_native_hooks = this;
	}

	native_hooks::~native_hooks()
	{
		m_native_hooks.clear();
		g_native_hooks = nullptr;
	}

	void native_hooks::add_native_detour(rage::scrNativeHash hash, rage::scrNativeHandler detour)
	{
		add_native_detour(ALL_SCRIPT_HASH, hash, detour);
	}

	void native_hooks::add_native_detour(rage::joaat_t script_hash, rage::scrNativeHash hash, rage::scrNativeHandler detour)
	{
		if (const auto& it = m_native_registrations.find(script_hash); it != m_native_registrations.end())
		{
			it->second.emplace_back(hash, detour);
			return;
		}

		m_native_registrations.emplace(script_hash, std::vector<native_detour>({{hash, detour}}));
	}

	void native_hooks::hook_program(rage::scrProgram* program)
	{
		std::unordered_map<rage::scrNativeHash, rage::scrNativeHandler> native_replacements;
		const auto script_hash = program->m_name_hash;

		// Functions that need to be detoured for all scripts
		if (const auto& pair = m_native_registrations.find(ALL_SCRIPT_HASH); pair != m_native_registrations.end())
			for (const auto& native_hook_reg : pair->second)
				native_replacements.insert(native_hook_reg);

		// Functions that only need to be detoured for a specific script
		if (const auto& pair = m_native_registrations.find(script_hash); pair != m_native_registrations.end())
			for (const auto& native_hook_reg : pair->second)
				native_replacements.insert(native_hook_reg);

		if (!native_replacements.empty())
		{
			m_native_hooks.emplace(program, std::make_unique<native_hook>(program, native_replacements));
		}
	}
}
