#pragma once
#include "gta/joaat.hpp"
#include "gta_util.hpp"
#include "math.hpp"
#include "natives.hpp"
#include "script.hpp"
#include "services/players/player_service.hpp"

namespace big::entity
{
	bool take_control_of(Entity ent, int n_of_tries = 5);
	bool network_has_control_of_entity(rage::netObject* net_object);
}
