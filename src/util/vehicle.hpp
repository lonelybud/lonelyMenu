#pragma once
#include "entity.hpp"
#include "gta/enums.hpp"
#include "natives.hpp"
#include "services/gta_data/vehicle_item.hpp"

namespace big::vehicle
{
	inline void set_plate(Vehicle veh, const char* plate)
	{
		if (!entity::take_control_of(veh))
			return;

		if (plate != nullptr && plate[0] != 0)
			VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT(veh, plate);
	}
	inline void repair()
	{
		if (!ENTITY::IS_ENTITY_A_VEHICLE(self::veh))
			return;

		VEHICLE::SET_VEHICLE_FIXED(self::veh);
		VEHICLE::SET_VEHICLE_DIRT_LEVEL(self::veh, 0.f);
	}

	Vector3 get_spawn_location(Hash hash, Ped ped = self::ped);

	Vehicle spawn(Hash hash, Vector3 location, float heading = 0, bool is_networked = true, bool script_veh = false);

	std::map<int, int32_t> get_owned_mods_from_vehicle(Vehicle vehicle);

	void max_vehicle(Vehicle veh);
	void max_vehicle_performance(Vehicle veh);

	void operate_vehicle_door(Vehicle veh, eDoorId doorId, bool open);

	inline std::string get_vehicle_model_name(vehicle_item veh)
	{
		return std::format("{} {} {}", veh.m_vehicle_class, veh.m_display_manufacturer, veh.m_display_name);
	}
	std::string get_vehicle_model_name(Vehicle veh);
}
