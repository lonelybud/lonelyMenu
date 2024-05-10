#pragma once
#include "backend/bool_command.hpp"
#include "backend/looped/looped.hpp"
#include "core/data/context_menu.hpp"
#include "core/data/self.hpp"

namespace big
{
	void looped::death_check()
	{
		if (g_local_player && g_local_player->m_player_info->m_game_state == eGameState::Died)
		{
			if (g_self.spectating)
			{
				g_self.spectating = false;
				dynamic_cast<bool_command*>(command::get("spectate"_J))->refresh();
			}
			if (g_self.free_cam)
			{
				g_self.free_cam = false;
				dynamic_cast<bool_command*>(command::get("freecam"_J))->refresh();
			}
			if (g_context_menu.enabled)
				g_context_menu.enabled = false;
		}
	}

	// inline void death_blame()
	// {
	// 	static bool death_source_identified = false;
	// 	auto m_game_session_state           = gta_util::get_network()->m_game_session_state;

	// 	if ((m_game_session_state == 0 || m_game_session_state > 4) && g_local_player->m_player_info->m_game_state == eGameState::Died)
	// 	{
	// 		if (death_source_identified)
	// 			return;

	// 		if (auto ent = PED::GET_PED_SOURCE_OF_DEATH(self::ped); ent)
	// 		{
	// 			death_source_identified = true;
	// 			Ped ped                 = 0;

	// 			if (ENTITY::IS_ENTITY_A_PED(ent))
	// 				ped = ent;
	// 			else if (ENTITY::IS_ENTITY_A_VEHICLE(ent))
	// 				ped = VEHICLE::GET_PED_IN_VEHICLE_SEAT(ent, -1, 0);

	// 			if (ped && PED::IS_PED_A_PLAYER(ped))
	// 				if (auto player = get_player_from_ped(ped))
	// 					LOG(WARNING) << "You got Killed by: " << player->get_name();
	// 		}

	// 		if (g_local_player->m_vehicle)
	// 			g_local_player->m_vehicle->m_door_lock_status = (int)eVehicleLockState::VEHICLELOCK_LOCKED;
	// 	}
	// 	else if (death_source_identified)
	// 		death_source_identified = false;
	// }
}