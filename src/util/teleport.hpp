#pragma once
#include "blip.hpp"
#include "entity.hpp"
#include "gta/enums.hpp"
#include "services/notifications/notification_service.hpp"

namespace big::teleport
{
	inline bool to_coords(Vector3 location, bool load_ground = false)
	{
		if (load_ground && !entity::load_ground_at_3dcoord(location))
		{
			g_notification_service->push_warning("Teleport", "Unable to load ground");
			return false;
		}

		auto yaw   = ENTITY::GET_ENTITY_HEADING(self::ped);
		auto pitch = CAM::GET_GAMEPLAY_CAM_RELATIVE_PITCH();
		auto roll  = CAM::GET_GAMEPLAY_CAM_RELATIVE_HEADING();

		PED::SET_PED_COORDS_KEEP_VEHICLE(self::ped, location.x, location.y, location.z + 1.f);

		if (self::veh)
		{
			ENTITY::SET_ENTITY_HEADING(self::veh, yaw);
			CAM::SET_GAMEPLAY_CAM_RELATIVE_PITCH(pitch, 1.f);
			CAM::SET_GAMEPLAY_CAM_RELATIVE_HEADING(roll);
		}

		return true;
	}

	inline bool to_waypoint()
	{
		std::optional<Vector3> location = blip::get_blip_location((int)BlipIcons::Waypoint);

		if (location.has_value())
			return to_coords(location.value(), true);

		return false;
	}

	inline bool to_objective()
	{
		std::optional<Vector3> location = blip::get_objective_location();

		if (location.has_value())
			return to_coords(location.value());

		return false;
	}

	inline void tp_on_top(Entity ent)
	{
		if (!ENTITY::DOES_ENTITY_EXIST(ent))
			return;

		Vector3 ent_dimensions_max{}, ent_dimensions_min{}, ent_pos{};

		MISC::GET_MODEL_DIMENSIONS(ENTITY::GET_ENTITY_MODEL(ent), &ent_dimensions_min, &ent_dimensions_max);
		ent_pos = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(ent, 0, 0, ent_dimensions_max.z);

		to_coords(ent_pos);
	}

	inline void into_vehicle(Vehicle veh)
	{
		if (!ENTITY::IS_ENTITY_A_VEHICLE(veh))
			return g_notification_service->push_warning("Teleport", "Invalid Vehicle");

		for (int i = -1; i < VEHICLE::GET_VEHICLE_MAX_NUMBER_OF_PASSENGERS(veh); i++)
			if (VEHICLE::IS_VEHICLE_SEAT_FREE(veh, i, TRUE))
			{
				if (to_coords(ENTITY::GET_ENTITY_COORDS(veh, 0)))
				{
					script::get_current()->yield();
					if (VEHICLE::IS_VEHICLE_SEAT_FREE(veh, i, TRUE))
						PED::SET_PED_INTO_VEHICLE(self::ped, veh, i);
				}
				return;
			}

		g_notification_service->push_warning("Teleport", "No seat available");
	}
}
