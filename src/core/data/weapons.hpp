#pragma once

namespace big
{
	struct g_weapons_t
	{
		bool infinite_mag = false;

		struct aimbot
		{
			bool enable    = false;
			bool player    = false;
			float distance = 1000.f;
		} aimbot{};
	};

	inline g_weapons_t g_weapons{};
}