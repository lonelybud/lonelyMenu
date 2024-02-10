#pragma once
#include "services/bad_players/bad_players.hpp"
#include "services/players/player.hpp"
#include "util/outfit.hpp"
#include "util/time.hpp"

namespace big
{
	inline player_ptr get_player_from_ped(Ped ped)
	{
		for (auto& p : g_player_service->players())
			if (p.second->get_ped())
				if (p.second->get_ped() == g_pointers->m_gta.m_handle_to_ptr(ped))
					return p.second;

		return nullptr;
	}

	inline void steal_player_outfit(big::player_ptr plyr, bool save = false)
	{
		Ped target;

		if (plyr->is_valid() && (target = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(plyr->id())))
		{
			if (ENTITY::GET_ENTITY_MODEL(target) != ENTITY::GET_ENTITY_MODEL(self::ped))
			{
				g_notification_service->push_error("Steal Oufit", "Model not same.", true);
				return;
			}

			if (save)
				outfit::save_outfit(target, std::to_string(get_current_time_in_mill()).append(".json"), "");
			else
				outfit::set_self_comps_props({}, {}, target);
		}
	}

	inline bool player_is_driver(player_ptr target_plyr)
	{
		if (auto driver =
		        g_local_player->m_vehicle->m_driver ? g_local_player->m_vehicle->m_driver : g_local_player->m_vehicle->m_last_driver)
		{
			// driver is a player present in session
			if (driver->m_player_info && g_player_service->get_by_host_token(driver->m_player_info->m_net_player_data.m_host_token))
				return driver == target_plyr->get_ped();

			// driver exists but is not a valid player
			return true;
		}

		return false;
	}
}