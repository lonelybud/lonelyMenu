#pragma once
#include "gta/enums.hpp"
#include "ped/CPedBoneInfo.hpp"

namespace big
{
	struct g_weapons_t
	{
		bool infinite_ammo    = false;
		bool infinite_mag     = false;
		bool rapid_fire       = false;
		int rapid_fire_delay  = 500;
		bool no_recoil_spread = false;

		struct aimbot
		{
			bool enable_old               = false;
			bool enable                   = false;
			bool player                   = false;
			ePedBoneType bone             = ePedBoneType::HEAD;
			float distance                = 1000.f;
			float max_dist_to_mid_of_scrn = 0.05f;
		} aimbot{};

		struct modify_weapon_values
		{
			bool enable         = false;
			float time_bw_shots = 0;
		} modify_weapon_values{};
	};

	inline g_weapons_t g_weapons{};
}