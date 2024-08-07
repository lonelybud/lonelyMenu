#include "vehicle.hpp"

#include "backend/player_command.hpp"
#include "core/vars.hpp"
#include "entity.hpp"
#include "player.hpp"
#include "services/gta_data/gta_data_service.hpp"

namespace big::vehicle
{
	std::optional<Vector3> get_spawn_location(Hash hash, Ped ped)
	{
		if (!entity::request_model(hash))
			return std::nullopt;

		Vector3 min, max, result;
		MISC::GET_MODEL_DIMENSIONS(hash, &min, &max);

		return ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(ped, 0.f, (max - min).y, 0.f);
	}

	Vehicle spawn(Hash hash, Vector3 location, float heading, bool is_networked, bool script_veh)
	{
		if (!entity::request_model(hash))
			return -1;

		if (!heading)
			heading = ENTITY::GET_ENTITY_HEADING(self::ped) + 90;

		auto veh = VEHICLE::CREATE_VEHICLE(hash, location.x, location.y, location.z + 1.f, heading, is_networked, script_veh, false);

		STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);

		auto network_id = NETWORK::VEH_TO_NET(veh);

		if (*g_pointers->m_gta.m_is_session_started && is_networked)
		{
			DECORATOR::DECOR_SET_INT(veh, "MPBitset", 0);

			if (NETWORK::NETWORK_GET_ENTITY_IS_NETWORKED(veh))
				NETWORK::SET_NETWORK_ID_EXISTS_ON_ALL_MACHINES(network_id, TRUE);

			VEHICLE::SET_VEHICLE_IS_STOLEN(veh, FALSE);
		}

		g_vehicle.spawned_vehicles[veh] = {vehicle::get_vehicle_model_name(veh)};
		add_blip_to_spawned_veh(veh, g_vehicle.spawned_vehicles[veh]);

		return veh;
	}

	std::map<int, int32_t> get_owned_mods_from_vehicle(Vehicle vehicle)
	{
		std::map<int, int32_t> owned_mods;

		for (int i = MOD_SECONDARY_CUSTOM; i <= MOD_MODEL_HASH; i++)
		{
			owned_mods[i] = 0;
		}

		owned_mods[MOD_MODEL_HASH] = ENTITY::GET_ENTITY_MODEL(vehicle);

		owned_mods[MOD_PLATE_STYLE] = VEHICLE::GET_VEHICLE_NUMBER_PLATE_TEXT_INDEX(vehicle);
		owned_mods[MOD_WINDOW_TINT] = VEHICLE::GET_VEHICLE_WINDOW_TINT(vehicle);
		owned_mods[MOD_WHEEL_TYPE]  = VEHICLE::GET_VEHICLE_WHEEL_TYPE(vehicle);

		VEHICLE::GET_VEHICLE_COLOURS(vehicle, &owned_mods[MOD_PRIMARY_COL], &owned_mods[MOD_SECONDARY_COL]);
		VEHICLE::GET_VEHICLE_EXTRA_COLOURS(vehicle, &owned_mods[MOD_PEARLESCENT_COL], &owned_mods[MOD_WHEEL_COL]);
		VEHICLE::GET_VEHICLE_EXTRA_COLOUR_5(vehicle, &owned_mods[MOD_INTERIOR_COL]);
		VEHICLE::GET_VEHICLE_EXTRA_COLOUR_6(vehicle, &owned_mods[MOD_DASHBOARD_COL]);

		owned_mods[MOD_PRIMARY_CUSTOM] = VEHICLE::GET_IS_VEHICLE_PRIMARY_COLOUR_CUSTOM(vehicle);
		if (owned_mods[MOD_PRIMARY_CUSTOM])
		{
			VEHICLE::GET_VEHICLE_CUSTOM_PRIMARY_COLOUR(vehicle, &owned_mods[MOD_PRIMARY_COL_R], &owned_mods[MOD_PRIMARY_COL_G], &owned_mods[MOD_PRIMARY_COL_B]);
		}

		owned_mods[MOD_SECONDARY_CUSTOM] = VEHICLE::GET_IS_VEHICLE_SECONDARY_COLOUR_CUSTOM(vehicle);
		if (owned_mods[MOD_SECONDARY_CUSTOM])
		{
			VEHICLE::GET_VEHICLE_CUSTOM_SECONDARY_COLOUR(vehicle, &owned_mods[MOD_SECONDARY_COL_R], &owned_mods[MOD_SECONDARY_COL_G], &owned_mods[MOD_SECONDARY_COL_B]);
		}

		owned_mods[MOD_TYRE_SMOKE] = VEHICLE::IS_TOGGLE_MOD_ON(vehicle, MOD_TYRE_SMOKE);
		if (owned_mods[MOD_TYRE_SMOKE])
		{
			VEHICLE::GET_VEHICLE_TYRE_SMOKE_COLOR(vehicle, &owned_mods[MOD_TIRESMOKE_COL_R], &owned_mods[MOD_TIRESMOKE_COL_G], &owned_mods[MOD_TIRESMOKE_COL_B]);
		}

		owned_mods[MOD_XENON_LIGHTS] = VEHICLE::IS_TOGGLE_MOD_ON(vehicle, MOD_XENON_LIGHTS);
		if (owned_mods[MOD_XENON_LIGHTS])
		{
			owned_mods[MOD_XENON_COL] = (int8_t)VEHICLE::GET_VEHICLE_XENON_LIGHT_COLOR_INDEX(vehicle);
		}

		owned_mods[MOD_NEON_LEFT_ON]  = VEHICLE::GET_VEHICLE_NEON_ENABLED(vehicle, NEON_LEFT);
		owned_mods[MOD_NEON_RIGHT_ON] = VEHICLE::GET_VEHICLE_NEON_ENABLED(vehicle, NEON_RIGHT);
		owned_mods[MOD_NEON_FRONT_ON] = VEHICLE::GET_VEHICLE_NEON_ENABLED(vehicle, NEON_FRONT);
		owned_mods[MOD_NEON_BACK_ON]  = VEHICLE::GET_VEHICLE_NEON_ENABLED(vehicle, NEON_BACK);
		VEHICLE::GET_VEHICLE_NEON_COLOUR(vehicle, &owned_mods[MOD_NEON_COL_R], &owned_mods[MOD_NEON_COL_G], &owned_mods[MOD_NEON_COL_B]);

		owned_mods[MOD_TIRE_CAN_BURST] = VEHICLE::GET_VEHICLE_TYRES_CAN_BURST(vehicle);
		owned_mods[MOD_DRIFT_TIRE]     = VEHICLE::GET_DRIFT_TYRES_SET(vehicle);
		owned_mods[MOD_TURBO]          = VEHICLE::IS_TOGGLE_MOD_ON(vehicle, MOD_TURBO);

		owned_mods[MOD_FRONTWHEEL_VAR] = VEHICLE::GET_VEHICLE_MOD_VARIATION(vehicle, MOD_FRONTWHEEL);
		owned_mods[MOD_REARWHEEL_VAR]  = VEHICLE::GET_VEHICLE_MOD_VARIATION(vehicle, MOD_REARWHEEL);

		for (int slot = MOD_SPOILERS; slot <= MOD_LIGHTBAR; slot++)
			if (VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, slot) > 0)
				owned_mods[slot] = VEHICLE::GET_VEHICLE_MOD(vehicle, slot);

		for (int extra = MOD_EXTRA_14; extra <= MOD_EXTRA_1; ++extra)
			if (auto id = (extra - MOD_EXTRA_1) * -1; VEHICLE::DOES_EXTRA_EXIST(vehicle, id))
				owned_mods[extra] = VEHICLE::IS_VEHICLE_EXTRA_TURNED_ON(vehicle, id);

		return owned_mods;
	}

	void max_vehicle(Vehicle veh)
	{
		if (!entity::take_control_of(veh))
			return;

		Hash model = ENTITY::GET_ENTITY_MODEL(veh);

		VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0);

		VEHICLE::TOGGLE_VEHICLE_MOD(veh, MOD_TURBO, TRUE);

		for (int slot = MOD_SPOILERS; slot <= MOD_LIGHTBAR; slot++)
		{
			if (slot == MOD_LIVERY)
				continue;

			int count = VEHICLE::GET_NUM_VEHICLE_MODS(veh, slot);
			if (count > 0)
			{
				int selected_mod = -1;

				for (int mod = count - 1; mod >= -1; mod--)
				{
					if (VEHICLE::IS_VEHICLE_MOD_GEN9_EXCLUSIVE(veh, slot, mod))
						continue;

					selected_mod = mod;
					break;
				}

				if (selected_mod != -1)
					VEHICLE::SET_VEHICLE_MOD(veh, slot, selected_mod, true);
			}
		}
	}

	void max_vehicle_performance(Vehicle veh)
	{
		if (entity::take_control_of(veh))
		{
			VehicleModType perfomance_mods[] = {MOD_ENGINE, MOD_BRAKES, MOD_TRANSMISSION, MOD_SUSPENSION, MOD_ARMOR, MOD_TURBO};
			VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0);

			for (auto mod_slot : perfomance_mods)
			{
				if (mod_slot != MOD_TURBO)
					VEHICLE::SET_VEHICLE_MOD(veh, mod_slot, VEHICLE::GET_NUM_VEHICLE_MODS(veh, mod_slot) - 1, true);
				else
					VEHICLE::TOGGLE_VEHICLE_MOD(veh, mod_slot, true);
			}
		}
	}

	/*
	* Set 'open' to false to close the door.
	* Set doorId to eDoorId::VEH_EXT_DOOR_INVALID_ID or simply -1 to apply to all doors.
	*/
	void operate_vehicle_door(Vehicle veh, eDoorId doorId, bool open)
	{
		if (!entity::take_control_of(veh))
			return;

		for (int i = 0; i < 6; i++)
			if ((doorId == eDoorId::VEH_EXT_DOOR_INVALID_ID || (int)doorId == i) && VEHICLE::GET_IS_DOOR_VALID(veh, i))
				if (open)
					VEHICLE::SET_VEHICLE_DOOR_OPEN(veh, i, false, false);
				else
					VEHICLE::SET_VEHICLE_DOOR_SHUT(veh, i, false);
	}

	std::string get_vehicle_model_name(Vehicle veh)
	{
		auto model = ENTITY::GET_ENTITY_MODEL(veh);
		auto& vehs = g_gta_data_service.vehicles();

		if (auto it = vehs.find(model); it != vehs.end())
			return get_vehicle_model_name(it->second);

		return "Unknown";
	}

	bool clear_all_peds(Vehicle vehicle)
	{
		if (auto passengers = VEHICLE::GET_VEHICLE_NUMBER_OF_PASSENGERS(vehicle, 1, 0))
		{
			auto players_count = 0;

			for (int i = -1; i < VEHICLE::GET_VEHICLE_MAX_NUMBER_OF_PASSENGERS(vehicle); ++i)
			{
				auto ped = VEHICLE::GET_PED_IN_VEHICLE_SEAT(vehicle, i, 0);
				if (PED::IS_PED_A_PLAYER(ped))
				{
					if (auto plyr = get_player_from_ped(ped))
						dynamic_cast<player_command*>(command::get("vehkick"_J))->call(plyr);
					
					++players_count;
				}
				// else if (entity::take_control_of(ped))
				// 	TASK::CLEAR_PED_TASKS_IMMEDIATELY(ped);
				// else
				// 	return false;
			}

			// wait for players to leave
			for (int i = 0; i < 13; ++i)
			{
				if (VEHICLE::GET_VEHICLE_NUMBER_OF_PASSENGERS(vehicle, 1, 0) != (passengers - players_count))
					script::get_current()->yield(150ms);
				else
					return true;
			}

			return false;
		}

		return true;
	}
}
