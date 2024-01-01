#pragma once

namespace big
{
	inline struct g_vehicle_t
	{
		bool god_mode          = false;
		bool veh_boost         = false;
		bool spawn_at_waypoint = false;
		bool force_rocket      = false;
		bool unlimited_weapons = false;

	} g_vehicle{};
}
