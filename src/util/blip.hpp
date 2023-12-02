#pragma once
#include "entity.hpp"
#include "gta/enums.hpp"
#include "natives.hpp"
#include "services/notifications/notification_service.hpp"

namespace big::blip
{
	inline std::optional<Vector3> get_waypoint_location()
	{
		auto blip = HUD::GET_FIRST_BLIP_INFO_ID((int)BlipIcons::Waypoint);

		if (!HUD::DOES_BLIP_EXIST(blip))
		{
			g_notification_service->push_warning("Blip", "No waypoint found");
			return std::nullopt;
		}

		auto location = HUD::GET_BLIP_COORDS(blip);

		if (!entity::load_ground_at_3dcoord(location))
		{
			g_notification_service->push_warning("Blip", "Unable to load ground");
			return std::nullopt;
		}

		return location;
	}
}