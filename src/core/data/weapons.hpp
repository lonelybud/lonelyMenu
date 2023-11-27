#pragma once

namespace big
{
	inline struct g_weapons
	{
		struct aimbot
		{
			bool enable;
			bool player;
			float distance = 1000.f;
		} aimbot{};
	} g_weapons{};
}