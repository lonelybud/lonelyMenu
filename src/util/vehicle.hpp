#pragma once
#include "backend/command.hpp"
#include "backend/player_command.hpp"
#include "entity.hpp"
#include "ped.hpp"
#include "script_global.hpp"

namespace big::vehicle
{
	inline void repair()
	{
		if (!ENTITY::IS_ENTITY_A_VEHICLE(self::veh))
			return;

		VEHICLE::SET_VEHICLE_FIXED(self::veh);
		VEHICLE::SET_VEHICLE_DIRT_LEVEL(self::veh, 0.f);
	}
}
