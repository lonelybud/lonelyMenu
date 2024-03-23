#pragma once
#include "entity.hpp"
#include "gta/enums.hpp"
#include "natives.hpp"
#include "services/notifications/notification_service.hpp"

namespace big::blip
{
	inline std::optional<Vector3> get_blip_location(int sprite, int color = -1, bool notify = true, bool load_ground = false)
	{
		Blip blip = HUD::GET_FIRST_BLIP_INFO_ID(sprite);

		if (color != -1)
			while (HUD::DOES_BLIP_EXIST(blip) && HUD::GET_BLIP_COLOUR(blip) != color)
				blip = HUD::GET_NEXT_BLIP_INFO_ID(sprite);

		if (!HUD::DOES_BLIP_EXIST(blip) || (color != -1 && HUD::GET_BLIP_COLOUR(blip) != color))
		{
			if (notify)
				g_notification_service.push_warning("Blip", "No waypoint found");
			return std::nullopt;
		}

		auto location = HUD::GET_BLIP_COORDS(blip);

		if (load_ground && !entity::load_ground_at_3dcoord(location))
		{
			if (notify)
				g_notification_service.push_warning("Blip", "Unable to load ground");
			return std::nullopt;
		}

		return location;
	}

	inline std::optional<Vector3> get_objective_location()
	{
		std::optional<Vector3> location;

		if (location = get_blip_location((int)BlipIcons::Circle, (int)BlipColors::YellowMission, false); location != std::nullopt)
			return location;
		if (location = get_blip_location((int)BlipIcons::Circle, (int)BlipColors::YellowMission2, false); location != std::nullopt)
			return location;
		if (location = get_blip_location((int)BlipIcons::Circle, (int)BlipColors::Mission, false); location != std::nullopt)
			return location;

		g_notification_service.push_warning("Blip", "No objective found");
		return std::nullopt;
	}
}