#pragma once

namespace big
{
	inline struct g_self_t
	{
		bool unlimited_oxygen         = false;
		bool god_mode                 = false;
		bool off_radar                = false;
		bool no_idle_kick             = false;
		bool spectating               = false;
		bool free_cam                 = false;
		bool never_wanted             = false;
		bool noclip                   = false;
		float noclip_speed_multiplier = 20.f;

		float custom_coords[3];
	} g_self{};
}