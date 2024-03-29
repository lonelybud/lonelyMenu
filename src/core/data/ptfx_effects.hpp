#pragma once

namespace big
{
	inline struct g_ptfx_effects
	{
		bool show          = false;
		float size         = 0.2f;
		const char* asset  = "scr_agencyheist";
		const char* effect = "scr_fbi_mop_drips";
		int delay          = 500;
		bool center        = false;
	} g_ptfx_effects{};
}