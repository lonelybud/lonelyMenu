#pragma once
#include "services/players/player.hpp"

namespace big::protection
{
	bool is_crash_object(rage::joaat_t model, player_ptr p);
	bool is_crash_ped(rage::joaat_t model);
	bool is_crash_vehicle(rage::joaat_t model);
	bool is_cage_object(rage::joaat_t model);
	bool is_valid_player_model(rage::joaat_t model);
}
