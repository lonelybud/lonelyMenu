#pragma once

namespace big
{
	struct g_weapons_t
	{
		bool infinite_mag    = false;
		bool rapid_fire      = false;
		int rapid_fire_delay = 0;

		struct aimbot
		{
			bool enable    = false;
			bool player    = false;
			float distance = 1000.f;
		} aimbot{};

		struct modify_weapon_values
		{
			bool enable         = false;
			float time_bw_shots = 0;
		} modify_weapon_values{};
	};

	inline g_weapons_t g_weapons{};
}