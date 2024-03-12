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

	inline bool force_migration(const char* name, rage::joaat_t hash, bool block_migration = false)
	{
		if (auto script = gta_util::find_script_thread(hash); script && script->m_net_component)
		{
			for (int i = 0; !((CGameScriptHandlerNetComponent*)script->m_net_component)->is_local_player_host(); i++)
			{
				if (i > 200)
					return false;

				((CGameScriptHandlerNetComponent*)script->m_net_component)
				    ->send_host_migration_event(g_player_service->get_self()->get_net_game_player());
				script::get_current()->yield(10ms);

				if (!script->m_stack || !script->m_net_component)
					return false;
			}
		}

		if (auto script = gta_util::find_script_thread(hash); script && script->m_net_component)
			((CGameScriptHandlerNetComponent*)script->m_net_component)->block_host_migration(block_migration);

		LOG(INFO) << name << " script host migration to you. " << (block_migration ? "(force)" : "(unforce)");

		return true;
	}
}
