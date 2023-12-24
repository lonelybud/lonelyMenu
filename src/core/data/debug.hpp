#pragma once

namespace big
{
	inline struct g_debug
	{
		bool fm_mission_controller_cart_grab = false;
		bool request_control                 = false;
		bool disable_clothing_validation     = false;
	} g_debug{};
}