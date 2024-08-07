#pragma once
#include "core/data/vehicle.hpp"
#include "entity.hpp"
#include "gta/enums.hpp"
#include "gta/vehicle_values.hpp"
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

	inline bool is_bennys(Vehicle veh)
	{
		auto v = VEHICLE::GET_VEHICLE_WHEEL_TYPE(veh);
		return v == WHEEL_TYPE_BENNYS_ORIGINAL || v == WHEEL_TYPE_BENNYS_BESPOKE || v == WHEEL_TYPE_OPEN_WHEEL || v == WHEEL_TYPE_STREET || v == WHEEL_TYPE_TRACK;
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

	bool clear_all_peds(Vehicle vehicle);

	inline void lockUnlockVehicle(Entity veh, eVehicleLockState current_state = eVehicleLockState::VEHICLELOCK_NONE)
	{
		if (entity::take_control_of(veh))
		{
			auto door_locked_state = current_state != eVehicleLockState::VEHICLELOCK_NONE ? current_state : (eVehicleLockState)VEHICLE::GET_VEHICLE_DOOR_LOCK_STATUS(veh);

			if (door_locked_state == eVehicleLockState::VEHICLELOCK_LOCKED)
			{
				VEHICLE::SET_VEHICLE_DOORS_LOCKED(veh, (int)eVehicleLockState::VEHICLELOCK_UNLOCKED);
				g_notification_service.push_success("Vehicle", "Vehicle is un-locked");
			}
			else
			{
				VEHICLE::SET_VEHICLE_DOORS_LOCKED(veh, (int)eVehicleLockState::VEHICLELOCK_LOCKED);
				g_notification_service.push_success("Vehicle", "Vehicle is locked");
			}
		}
		else
			g_notification_service.push_warning("Toxic", "Failed to take control of vehicle.");
	}

	inline void add_blip_to_spawned_veh(Vehicle veh, g_vehicle_t::SpawnedVehicle& v)
	{
		Blip blip = HUD::ADD_BLIP_FOR_ENTITY(veh);
		HUD::SET_BLIP_DISPLAY(blip, 2);
		HUD::SET_BLIP_SPRITE(blip, 225);
		HUD::SET_BLIP_COLOUR(blip, 46);
		HUD::SET_BLIP_AS_SHORT_RANGE(blip, FALSE);
		HUD::BEGIN_TEXT_COMMAND_SET_BLIP_NAME("STRING");
		HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Spawned Vehicle");
		HUD::END_TEXT_COMMAND_SET_BLIP_NAME(blip);
		v.blip = blip;
	}

	inline void update_veh_ammo(Vehicle veh, int amount)
	{
		if (ENTITY::DOES_ENTITY_EXIST(veh) && VEHICLE::DOES_VEHICLE_HAVE_WEAPONS(veh) && entity::take_control_of(veh))
		{
			for (int i = 0; i < 3; i++)
				VEHICLE::SET_VEHICLE_WEAPON_RESTRICTED_AMMO(veh, i, amount);
			VEHICLE::SET_VEHICLE_BOMB_AMMO(veh, amount);
			VEHICLE::SET_VEHICLE_COUNTERMEASURE_AMMO(veh, amount);

			g_notification_service.push_success("Vehicle Ammo", "Success updated");
		}
		else
			g_notification_service.push_error("Vehicle Ammo", "Failed to update");
	}
}
