#pragma once
#include "entity.hpp"
#include "gta/enums.hpp"
#include "natives.hpp"
#include "services/gta_data/vehicle_item.hpp"
#include "services/notifications/notification_service.hpp"

namespace big::vehicle
{
	inline float mps_to_miph(float mps)
	{
		return mps * 2.2369f;
	}
	inline float miph_to_mps(float miph)
	{
		return miph / 2.2369f;
	}

	inline void set_plate(Vehicle veh, const char* plate)
	{
		if (!entity::take_control_of(veh))
			return;

		if (plate != nullptr && plate[0] != 0)
			VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT(veh, plate);
	}
	inline void repair(Vehicle veh)
	{
		if (ENTITY::IS_ENTITY_A_VEHICLE(veh) && entity::take_control_of(veh))
		{
			VEHICLE::SET_VEHICLE_FIXED(veh);
			VEHICLE::SET_VEHICLE_DIRT_LEVEL(veh, 0.f);
		}
	}

	std::optional<Vector3> get_spawn_location(Hash hash, Ped ped);

	Vehicle spawn(Hash hash, Vector3 location, float heading = 0, bool is_networked = true, bool script_veh = false);

	std::map<int, int32_t> get_owned_mods_from_vehicle(Vehicle vehicle);

	void max_vehicle(Vehicle veh);
	void max_vehicle_performance(Vehicle veh);

	void operate_vehicle_door(Vehicle veh, eDoorId doorId, bool open);

	inline std::string get_vehicle_model_name(const vehicle_item veh)
	{
		return std::format("{} {} {}", veh.m_vehicle_class, veh.m_display_manufacturer, veh.m_display_name);
	}
	std::string get_vehicle_model_name(Vehicle veh);

	inline bool clear_all_peds(Vehicle vehicle)
	{
		if (auto passengers = VEHICLE::GET_VEHICLE_NUMBER_OF_PASSENGERS(vehicle, 1, 0))
		{
			// npcs
			for (int i = -1; i < VEHICLE::GET_VEHICLE_MAX_NUMBER_OF_PASSENGERS(vehicle); ++i)
			{
				auto ped = VEHICLE::GET_PED_IN_VEHICLE_SEAT(vehicle, i, 0);
				if (PED::IS_PED_A_PLAYER(ped))
					return false;
				else
					TASK::CLEAR_PED_TASKS_IMMEDIATELY(ped);
			}

			// wait for passengers to leave
			for (int i = 0; i < 10; ++i)
			{
				if (VEHICLE::GET_VEHICLE_NUMBER_OF_PASSENGERS(vehicle, 1, 0) != 0)
					script::get_current()->yield(150ms);
				else
					return true;
			}

			return false;
		}

		return true;
	}

	inline void lockUnlockVehicle(Entity veh, eVehicleLockState current_state = eVehicleLockState::VEHICLELOCK_NONE)
	{
		if (entity::take_control_of(veh))
		{
			auto door_locked_state = current_state != eVehicleLockState::VEHICLELOCK_NONE ? current_state : (eVehicleLockState)VEHICLE::GET_VEHICLE_DOOR_LOCK_STATUS(veh);

			if (door_locked_state == eVehicleLockState::VEHICLELOCK_LOCKED)
			{
				VEHICLE::SET_VEHICLE_DOORS_LOCKED(veh, (int)eVehicleLockState::VEHICLELOCK_UNLOCKED);
				g_notification_service->push_success("Vehicle", "Vehicle is un-locked");
			}
			else
			{
				VEHICLE::SET_VEHICLE_DOORS_LOCKED(veh, (int)eVehicleLockState::VEHICLELOCK_LOCKED);
				g_notification_service->push_success("Vehicle", "Vehicle is locked");
			}
		}
		else
			g_notification_service->push_warning("Toxic", "Failed to take control of vehicle.");
	}
}
