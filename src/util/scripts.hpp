#pragma once
#include "gta/script_handler.hpp"
#include "gta_util.hpp"
#include "script.hpp"
#include "services/players/player_service.hpp"

namespace big::scripts
{
	inline void get_host_name(rage::joaat_t hash, std::string& name)
	{
		if (auto script = gta_util::find_script_thread(hash); script && script->m_net_component)
			if (auto host = ((CGameScriptHandlerNetComponent*)script->m_net_component)->get_host(); host)
				name = host->get_name();
	}

	inline CGameScriptHandlerNetComponent* get_script_handler_net_comp(rage::joaat_t hash)
	{
		if (auto script = gta_util::find_script_thread(hash); script && script->m_net_component)
			return ((CGameScriptHandlerNetComponent*)script->m_net_component);

		return nullptr;
	}

	inline bool force_migration(rage::joaat_t hash, bool block_migration = false)
	{
		if (auto script = gta_util::find_script_thread(hash); script && script->m_net_component)
			if (auto comp = (CGameScriptHandlerNetComponent*)script->m_net_component; comp->get_host())
			{
				std::string curr_host = comp->get_host()->get_name();

				for (int i = 0; !comp->is_local_player_host(); i++)
				{
					if (i > 200)
						return false;

					comp->send_host_migration_event(g_player_service->get_self()->get_net_game_player());
					script::get_current()->yield(10ms);

					if (!(script && script->m_net_component))
						return false;
				}

				comp->block_host_migration(block_migration);

				LOGF(INFO, "Script migration: {} from {} ({})", script->m_name, curr_host, (block_migration ? "(Locked)" : ""));

				return true;
			}

		return false;
	}

	inline const std::optional<uint32_t> get_code_location_by_pattern(rage::scrProgram* program, const memory::pattern& pattern)
	{
		uint32_t code_size = program->m_code_size;
		for (uint32_t i = 0; i < (code_size - pattern.m_bytes.size()); i++)
		{
			for (uint32_t j = 0; j < pattern.m_bytes.size(); j++)
				if (pattern.m_bytes[j].has_value())
					if (pattern.m_bytes[j].value() != *program->get_code_address(i + j))
						goto incorrect;

			return i;
		incorrect:
			continue;
		}

		return std::nullopt;
	}
}
