#pragma once
#include "services/players/player.hpp"
#include "util/lua_helper.hpp"
#include "util/teleport.hpp"

// https://github.com/SilentSal0/Silent-Night
// https://github.com/YimMenu-Lua/Casino-Pacino
// https://github.com/L7NEG/Ultimate-Menu/blob
// https://github.com/YimMenu-Lua/DailyCollectibles
// https://github.com/Deadlineem/Extras-Addon-for-YimMenu

namespace big
{
	static std::optional<Vector3> dead_drop_coords(int area, int location)
	{
		constexpr float a_l_c[15][5][3] = {
		    {{1113.557, -645.957, 56.091}, {1142.874, -662.951, 57.135}, {1146.691, -703.717, 56.167}, {1073.542, -678.236, 56.583}, {1046.454, -722.915, 56.419}},
		    {{2064.713, 3489.88, 44.223}, {2081.859, 3553.254, 42.157}, {2014.72, 3551.499, 42.726}, {1997.019, 3507.838, 39.666}, {2045.597, 3564.346, 39.343}},
		    {{-1317.344, -1481.97, 3.923}, {-1350.041, -1478.273, 4.567}, {-1393.87, -1445.139, 3.437}, {-1367.034, -1413.992, 2.611}, {-1269.861, -1426.272, 3.556}},
		    {{-295.468, 2787.385, 59.864}, {-284.69, 2848.234, 53.266}, {-329.193, 2803.404, 57.787}, {-306.847, 2825.6, 58.219}, {-336.046, 2829.988, 55.448}},
		    {{-1725.245, 233.946, 57.685}, {-1639.892, 225.521, 60.336}, {-1648.48, 212.049, 59.777}, {-1693.318, 156.665, 63.855}, {-1699.193, 179.574, 63.185}},
		    {{-949.714, -710.658, 19.604}, {-938.774, -781.817, 19.657}, {-884.91, -786.863, 15.043}, {-895.257, -729.943, 19.143}, {-932.986, -746.452, 19.008}},
		    {{-425.948, 1213.342, 324.936}, {-387.267, 1137.65, 321.704}, {-477.999, 1135.36, 320.123}, {-431.822, 1119.449, 325.964}, {-387.902, 1161.655, 324.529}},
		    {{-3381.278, 965.534, 7.426}, {-3427.724, 979.944, 7.526}, {-3413.606, 961.845, 11.038}, {-3419.585, 977.595, 11.167}, {-3425.687, 961.215, 7.536}},
		    {{-688.732, 5828.4, 16.696}, {-673.425, 5799.744, 16.467}, {-710.348, 5769.631, 16.75}, {-699.926, 5801.619, 16.504}, {-660.359, 5781.733, 18.774}},
		    {{38.717, 6264.173, 32.88}, {84.67, 6292.286, 30.731}, {97.17, 6288.558, 38.447}, {14.453, 6243.932, 35.445}, {67.52, 6261.744, 32.029}},
		    {{2954.598, 4671.458, 50.106}, {2911.146, 4637.608, 49.3}, {2945.212, 4624.044, 49.078}, {2941.139, 4617.117, 52.114}, {2895.884, 4686.396, 48.094}},
		    {{1332.319, 4271.446, 30.646}, {1353.332, 4387.911, 43.541}, {1337.892, 4321.563, 38.093}, {1386.603, 4366.511, 42.236}, {1303.193, 4313.509, 36.939}},
		    {{2720.03, 1572.762, 20.204}, {2663.161, 1581.395, 24.418}, {2661.482, 1641.057, 24.001}, {2671.003, 1561.394, 23.882}, {2660.104, 1606.54, 28.61}},
		    {{211.775, -934.269, 23.466}, {198.265, -884.039, 30.696}, {189.542, -919.726, 29.96}, {169.504, -934.841, 29.228}, {212.376, -934.807, 29.007}},
		    {{1330.113, -2520.754, 46.365}, {1328.954, -2538.302, 46.976}, {1237.506, -2572.335, 39.791}, {1244.602, -2563.721, 42.646}, {1278.421, -2565.117, 43.544}},
		};

		if (area >= 0 && area <= 14 && location >= 0 && location <= 4)
		{
			auto loc_arr = a_l_c[area][location];
			return Vector3(loc_arr[0], loc_arr[1], loc_arr[2]);
		}

		return std::nullopt;
	}

	namespace lua_scripts
	{
		// ******************************************************************************** variables & consts

		static constexpr int FMg = 262145; // free mode global ("CASH_MULTIPLIER")
		static constexpr int SCCg = FMg + 19321; // sex changer change appearance cooldown global ("CHARACTER_APPEARANCE_COOLDOWN")
		static constexpr int fm_mission_controller_cart_grab       = 10253;
		static constexpr int fm_mission_controller_cart_grab_speed = 14;
		static constexpr int CPFHl                                 = 24333; // cayo perico fingerprint hack local
		static constexpr int CPPCCl = 30357 + 3; // cayo perico plasma cutter cut local ("DLC_H4_anims_glass_cutter_Sounds")
		static constexpr int CPSTCl         = 29118;      // cayo perico Drainage Pipe cut local
		static constexpr int STRKM52        = 52985;      // diamond casino fingerprint hack local
		static constexpr int STRKM51        = 54047;      // diamond casino keypad hack local
		static constexpr int DCDVDl1        = 10107 + 7;  // diamond casino drill vault door local 1
		static constexpr int DCDVDl2        = 10107 + 37; // diamond casino drill vault door local 2
		static auto FMC                     = "fm_mission_controller";
		static auto FMC20                   = "fm_mission_controller_2020";
		inline const char* casino_targets[] = {"Money", "Gold", "Art", "Diamonds"};
		inline const char* cayo_targets[] = {"Tequila (630K)", "Ruby Necklace (700K)", "Bearer Bonds (770K)", "Pink Diamond (1.3M)", "Madrazo Files", "Panther Statue (1.9M)"};

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
			auto v = lua_helper::locals::get_int(FMC, STRKM52);
			if (v == 3 || v == 4)
				lua_helper::locals::set_int(FMC, STRKM52, 5);
		}
		inline void bypass_keypad_casino()
		{
			auto v = lua_helper::locals::get_int(FMC, STRKM51);
			if (v != 4 || v >= 3 || v < 100)
				lua_helper::locals::set_int(FMC, STRKM51, 5);
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
		inline void log_cayo_details()
		{
			std::string cayo_island_targets = "";
			if (lua_helper::stats::get_int("MPX_H4LOOT_CASH_I") > 0)
				cayo_island_targets += "Cash; ";
			if (lua_helper::stats::get_int("MPX_H4LOOT_WEED_I") > 0)
				cayo_island_targets += "Weed; ";
			if (lua_helper::stats::get_int("MPX_H4LOOT_COKE_I") > 0)
				cayo_island_targets += "Coke; ";

			std::string cayo_compound_targets = "";
			if (lua_helper::stats::get_int("MPX_H4LOOT_CASH_C") > 0)
				cayo_compound_targets += "Cash; ";
			if (lua_helper::stats::get_int("MPX_H4LOOT_GOLD_C") > 0)
				cayo_compound_targets += "Gold; ";
			if (lua_helper::stats::get_int("MPX_H4LOOT_PAINT") > 0)
				cayo_compound_targets += "Arts; ";

			LOGF(VERBOSE, "Cash:{}, weed:{}, coke:{}, gold:{}", lua_helper::stats::get_int("MPX_H4LOOT_CASH_V"), lua_helper::stats::get_int("MPX_H4LOOT_WEED_V"), lua_helper::stats::get_int("MPX_H4LOOT_COKE_V"), lua_helper::stats::get_int("MPX_H4LOOT_GOLD_V"));
			LOG(VERBOSE) << "Island Targets: " << cayo_island_targets;
			LOG(VERBOSE) << "Compound Targets: " << cayo_compound_targets;
			LOG(VERBOSE) << "Primary Target: " << cayo_targets[get_cayo_target()];
		}
		inline void set_compound_and_island_targets()
		{
			// ************ CayoTargetsSetter(0, 0, 0, 255, "C", "GOLD", 330350)
			lua_helper::stats::set_int("MPX_H4LOOT_GOLD_C", 255);
			lua_helper::stats::set_int("MPX_H4LOOT_CASH_C", 0);
			lua_helper::stats::set_int("MPX_H4LOOT_PAINT", 0);

			// ************ CayoTargetsSetter(0, 0, 16777215, 0, "I", "COKE", 200095)
			lua_helper::stats::set_int("MPX_H4LOOT_COKE_I", 16777215);
			lua_helper::stats::set_int("MPX_H4LOOT_CASH_I", 0);
			lua_helper::stats::set_int("MPX_H4LOOT_WEED_I", 0);
		}
		inline void scope_compound_and_island_targets()
		{
			// ************ CayoTargetsSetter(0, 0, 0, 255, "C", "GOLD", 330350)
			lua_helper::stats::set_int("MPX_H4LOOT_GOLD_C_SCOPED", 255);
			lua_helper::stats::set_int("MPX_H4LOOT_CASH_C_SCOPED", 0);
			lua_helper::stats::set_int("MPX_H4LOOT_PAINT_SCOPED", 0);

			// ************ CayoTargetsSetter(0, 0, 16777215, 0, "I", "COKE", 200095)
			lua_helper::stats::set_int("MPX_H4LOOT_COKE_I_SCOPED", 16777215);
			lua_helper::stats::set_int("MPX_H4LOOT_CASH_I_SCOPED", 0);
			lua_helper::stats::set_int("MPX_H4LOOT_WEED_I_SCOPED", 0);
		}
		inline void tp_to_g_cache_coords()
		{
			// https://github.com/YimMenu-Lua/DailyCollectibles
			auto is_dead_drop_collected = lua_helper::stats::get_packed_stat_bool(36628);
			auto dead_drop_area         = lua_helper::stats::get_packed_stat_int(41214);
			auto dead_drop_loc          = lua_helper::stats::get_packed_stat_int(41213);

			if (!is_dead_drop_collected)
			{
				std::optional<Vector3> location = dead_drop_coords(dead_drop_area, dead_drop_loc);
				if (location.has_value())
					teleport::to_coords(location.value());
			}
		}
		// no pv should be out
		// Gift Veh to player and ask him to replace the car in the garage (garage must be full)
		// ask him to add insurance afterwards he gets the car
		// https://github.com/Deadlineem/Extras-Addon-for-YimMenu
		inline void gift_veh(player_ptr plyr)
		{
			if (auto ped = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(plyr->id()))
			{
				if (Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(ped, 0); veh && entity::take_control_of(veh))
				{
					auto net_hash = NETWORK::NETWORK_HASH_FROM_PLAYER_HANDLE(plyr->id());

					DECORATOR::DECOR_SET_INT(veh, "MPBitset", 3);
					DECORATOR::DECOR_SET_INT(veh, "Previous_Owner", net_hash);
					DECORATOR::DECOR_SET_INT(veh, "Veh_Modded_By_Player", net_hash);
					DECORATOR::DECOR_SET_INT(veh, "Not_Allow_As_Saved_Veh", 0);
					DECORATOR::DECOR_SET_INT(veh, "Player_Vehicle", net_hash);
				}
				else
					g_notification_service.push_error("Gift Vehicle", "Failed to get veh", false);
			}
		}
		inline void allow_sex_change(bool is_sex_change_allowed)
		{
			lua_helper::stats::set_int("MPX_ALLOW_GENDER_CHANGE", is_sex_change_allowed ? 52 : 0);
		}
		inline void change_appearance_cooldown(bool v)
		{
			lua_helper::globals::set_int(SCCg, v ? 0 : 2880000);
		}
	}
}