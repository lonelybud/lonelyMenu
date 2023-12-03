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
		std::optional<Vector3> waypoint_location = blip::get_waypoint_location();

		if (waypoint_location.has_value())
			return to_coords(waypoint_location.value(), true);

		return false;
	}
}
