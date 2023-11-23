#pragma once
#include "backend/command.hpp"
#include "backend/player_command.hpp"
#include "entity.hpp"
#include "ped.hpp"
#include "script_global.hpp"

namespace big::vehicle
{
	inline Vehicle get_self_veh()
	{
		return PED::GET_VEHICLE_PED_IS_IN(ped::get_self_ped(), false);
	}

	inline void repair()
	{
		auto veh = get_self_veh();

		if (!ENTITY::IS_ENTITY_A_VEHICLE(veh))
			return;

		VEHICLE::SET_VEHICLE_FIXED(veh);
		VEHICLE::SET_VEHICLE_DIRT_LEVEL(veh, 0.f);
	}
}
