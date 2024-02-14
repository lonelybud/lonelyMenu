#pragma once

namespace big
{
	inline struct g_vehicle_t
	{
		bool god_mode          = false;
		bool veh_boost         = false;
		bool spawn_at_waypoint = false;
		bool spawn_inside      = false;
		bool force_rocket      = false;
		bool unlimited_weapons = false;
		bool block_homing      = false;

		bool vehicle_lock                    = false;
		bool vehicle_lock_unlock_temporarily = false;

		struct persist_vehicle_t
		{
			std::string folder, file;
			std::vector<std::string> folders, files;
		} persist_vehicle;

		struct SpawnedVehicle
		{
			std::string vehicle_name;
			int passenger_count = 0;
			bool is_locked      = false;
			bool is_dead        = false;
			Blip blip;
			bool is_networked = false;
		};

		std::map<int, SpawnedVehicle> spawned_vehicles;
	} g_vehicle{};
}
