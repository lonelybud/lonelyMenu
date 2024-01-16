#pragma once
#include "util/lua_helper.hpp"

// https://github.com/SilentSal0/Silent-Night
// https://github.com/YimMenu-Lua/Casino-Pacino

namespace big
{
	namespace lua_scripts
	{
		// ******************************************************************************** variables & consts

		static constexpr int fm_mission_controller_cart_grab       = 10253;
		static constexpr int fm_mission_controller_cart_grab_speed = 14;
		static constexpr int CPFHl                                 = 24333; // cayo perico fingerprint hack local
		static constexpr int CPPCCl = 30357 + 3; // cayo perico plasma cutter cut local ("DLC_H4_anims_glass_cutter_Sounds")
		static constexpr int CPSTCl         = 29118;      // cayo perico sewer tunnel cut local
		static constexpr int DCFHl          = 52985;      // diamond casino fingerprint hack local
		static constexpr int DCKHl          = 54047;      // diamond casino keypad hack local
		static constexpr int DCDVDl1        = 10107 + 7;  // diamond casino drill vault door local 1
		static constexpr int DCDVDl2        = 10107 + 37; // diamond casino drill vault door local 2
		static auto FMC                     = "fm_mission_controller";
		static auto FMC20                   = "fm_mission_controller_2020";
		inline const char* casino_targets[] = {"Money", "Gold", "Art", "Diamonds"};
		inline const char* cayo_targets[] = {"Tequila (630K)", "Ruby Necklace (700K)", "Bearer Bonds (770K)", "Pink Diamond (1.3M)", "Unkown", "Panther Statue (1.9M)"};

		struct CayoLocation
		{
			const char* name;
			float cord[3];
		};

		// ******************************************************************************** functions

		inline void casino_cart_item_auto_grab()
		{
			if (lua_helper::locals::get_int(FMC, fm_mission_controller_cart_grab) == 3)
				lua_helper::locals::set_int(FMC, fm_mission_controller_cart_grab, 4);
			else if (lua_helper::locals::get_int(FMC, fm_mission_controller_cart_grab) == 4)
				lua_helper::locals::set_float(FMC, fm_mission_controller_cart_grab + fm_mission_controller_cart_grab_speed, 2);
		}
		inline void increase_nightClub_popularity()
		{
			lua_helper::stats::set_int("MPX_CLUB_POPULARITY", 1000);
		}
		inline std::string get_stash_house_safecode()
		{
			std::string combination_retn = "";
			for (int i = 0; i <= 2; i++)
				if (i == 2)
					combination_retn += std::to_string(lua_helper::locals::get_int("fm_content_stash_house", 117 + 22 + (1 + (i * 2)) + 1));
				else
					combination_retn += std::to_string(lua_helper::locals::get_int("fm_content_stash_house", 117 + 22 + (1 + (i * 2)) + 1)) + "-";
			return combination_retn;
		}
		inline int get_casino_target()
		{
			return lua_helper::stats::get_int("MPX_H3OPT_TARGET");
		}
		inline void set_casino_target(int target)
		{
			lua_helper::stats::set_int("MPX_H3OPT_TARGET", target);
		}
		inline void bypass_fingerprint_casino()
		{
			if (lua_helper::locals::get_int(FMC, DCFHl) == 4)
				lua_helper::locals::set_int(FMC, DCFHl, 5);
		}
		inline void bypass_keypad_casino()
		{
			if (lua_helper::locals::get_int(FMC, DCKHl) != 4)
				lua_helper::locals::set_int(FMC, DCKHl, 5);
		}
		inline void bypass_drill()
		{
			lua_helper::locals::set_int(FMC, DCDVDl1, lua_helper::locals::get_int(FMC, DCDVDl2));
		}
		inline int get_cayo_target()
		{
			return lua_helper::stats::get_int("MPX_H4CNF_TARGET");
		}
		inline void set_cayo_target(int target)
		{
			lua_helper::stats::set_int("MPX_H4CNF_TARGET", target);
		}
		inline void bypass_fingerprint_cayo()
		{
			if (lua_helper::locals::get_int(FMC20, CPFHl) == 4)
				lua_helper::locals::set_int(FMC20, CPFHl, 5);
		}
		inline void bypass_plasma_cutter()
		{
			lua_helper::locals::set_float(FMC20, CPPCCl, 100);
		}
		inline void bypass_sewer_tunnel()
		{
			if (auto v = lua_helper::locals::get_int(FMC20, CPSTCl); v >= 3 || v <= 6)
				lua_helper::locals::set_int(FMC20, CPSTCl, 6);
		}
		inline void log_cayo_details()
		{
			LOG(WARNING) << "CASH Value: " << lua_helper::stats::get_int("MPX_H4LOOT_CASH_V");
			LOG(WARNING) << "WEED Value: " << lua_helper::stats::get_int("MPX_H4LOOT_WEED_V");
			LOG(WARNING) << "COKE Value: " << lua_helper::stats::get_int("MPX_H4LOOT_COKE_V");
			LOG(WARNING) << "GOLD Value: " << lua_helper::stats::get_int("MPX_H4LOOT_GOLD_V");

			std::string cayo_island_targets = "";
			if (lua_helper::stats::get_int("MPX_H4LOOT_CASH_I") > 0)
				cayo_island_targets += "Cash; ";
			if (lua_helper::stats::get_int("MPX_H4LOOT_WEED_I") > 0)
				cayo_island_targets += "Weed; ";
			if (lua_helper::stats::get_int("MPX_H4LOOT_COKE_I") > 0)
				cayo_island_targets += "Coke; ";
			LOG(WARNING) << "Island Targets: " << cayo_island_targets;

			std::string cayo_compound_targets = "";
			if (lua_helper::stats::get_int("MPX_H4LOOT_CASH_C") > 0)
				cayo_compound_targets += "Cash; ";
			if (lua_helper::stats::get_int("MPX_H4LOOT_GOLD_C") > 0)
				cayo_compound_targets += "Gold; ";
			if (lua_helper::stats::get_int("MPX_H4LOOT_PAINT") > 0)
				cayo_compound_targets += "Arts; ";
			LOG(WARNING) << "Compound Targets: " << cayo_compound_targets;
			LOG(WARNING) << "Primary Target: " << cayo_targets[get_cayo_target()];
		}
		inline void scope_compound_and_island_targets()
		{
			// ************ CayoTargetsSetter(0, 0, 0, 255, "C", "GOLD", 330350)
			lua_helper::stats::set_int("MPX_H4LOOT_GOLD_C", 255);
			lua_helper::stats::set_int("MPX_H4LOOT_GOLD_C_SCOPED", 255);
			lua_helper::stats::set_int("MPX_H4LOOT_CASH_C", 0);
			lua_helper::stats::set_int("MPX_H4LOOT_CASH_C_SCOPED", 0);
			lua_helper::stats::set_int("MPX_H4LOOT_PAINT", 0);
			lua_helper::stats::set_int("MPX_H4LOOT_PAINT_SCOPED", 0);

			// ************ CayoTargetsSetter(0, 0, 16777215, 0, "I", "COKE", 200095)
			lua_helper::stats::set_int("MPX_H4LOOT_COKE_I", 16777215);
			lua_helper::stats::set_int("MPX_H4LOOT_COKE_I_SCOPED", 16777215);
			lua_helper::stats::set_int("MPX_H4LOOT_CASH_I", 0);
			lua_helper::stats::set_int("MPX_H4LOOT_CASH_I_SCOPED", 0);
			lua_helper::stats::set_int("MPX_H4LOOT_WEED_I", 0);
			lua_helper::stats::set_int("MPX_H4LOOT_WEED_I_SCOPED", 0);
		}
	}
}