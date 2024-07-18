#pragma once
#include "gta/enums.hpp"
#include "ped/CPedBoneInfo.hpp"

namespace big
{
	struct g_weapons_t
	{
		bool infinite_ammo          = false;
		bool infinite_mag           = false;
		bool rapid_fire             = false;
		int rapid_fire_delay        = 500;
		bool no_recoil_spread       = false;
		bool ammo_special_type      = false;
		eExplosionTag explosion_tag = eExplosionTag::DONTCARE;

		struct aimbot
		{
			bool enable                   = false;
			bool enable_2                  = false;
			bool player                   = false;
			ePedBoneType bone             = ePedBoneType::HEAD;
			float max_dist_to_mid_of_scrn = 0.05f;
			bool check_velocity           = false;
		} aimbot{};

		struct modify_weapon_values
		{
			bool enable         = false;
			float time_bw_shots = 0;
		} modify_weapon_values{};
	};

	inline g_weapons_t g_weapons{};
}