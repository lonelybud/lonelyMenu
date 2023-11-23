#pragma once
#include "core/scr_globals.hpp"
#include "fiber_pool.hpp"
#include "gta/script_handler.hpp"
#include "gta_util.hpp"
#include "misc.hpp"
#include "natives.hpp"
#include "script.hpp"
#include "script_local.hpp"
#include "services/notifications/notification_service.hpp"
#include "services/players/player_service.hpp"

#include <memory/pattern.hpp>

namespace big::scripts
{
	inline bool force_migration(rage::joaat_t hash)
	{
		if (auto launcher = gta_util::find_script_thread(hash); launcher && launcher->m_net_component)
		{
			for (int i = 0; !((CGameScriptHandlerNetComponent*)launcher->m_net_component)->is_local_player_host(); i++)
			{
				if (i > 200)
					return false;

				((CGameScriptHandlerNetComponent*)launcher->m_net_component)
				    ->send_host_migration_event(g_player_service->get_self()->get_net_game_player());
				script::get_current()->yield(10ms);

				if (!launcher->m_stack || !launcher->m_net_component)
					return false;
			}
		}

		return true;
	}
}