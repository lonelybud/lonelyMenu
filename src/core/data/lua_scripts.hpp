#pragma once
#include "rage/joaat.hpp"
#include "services/players/player.hpp"
#include "util/lua_helper.hpp"
#include "util/teleport.hpp"

namespace big
{
	namespace lua_scripts
	{
		// ******************************************************************************** variables & consts
		static auto FMC    = "fm_mission_controller";
		static auto FMC20  = "fm_mission_controller_2020";
		static auto FMCIH  = "fm_content_island_heist";
		static auto FMCVRP = "fm_content_vehrob_prep";
		static auto AMACM  = "am_mp_arc_cab_manager";

		// https://github.com/YimMenu-Lua/Casino-Pacino/blob/main/CasinoPacino.lua
		inline const char* casino_targets[] = {"Money", "Gold", "Art", "Diamonds"};
		// https://github.com/SilentSal0/Silent-Night/blob/main/Silent%20Night%20v1.68.lua
		inline const char* cayo_targets[] = {"Tequila (630K)", "Ruby Necklace (700K)", "Bearer Bonds (770K)", "Pink Diamond (1.3M)", "Madrazo Files", "Panther Statue (1.9M)"};

		// ******************************************************************************** functions

		// https://github.com/YimMenu-Lua/Casino-Pacino/blob/main/CasinoPacino.lua
		inline void casino_cart_item_auto_grab()
		{
			constexpr int fm_mission_controller_cart_grab       = 10255;
			constexpr int fm_mission_controller_cart_grab_speed = 14;
			if (lua_helper::locals::get_int(FMC, fm_mission_controller_cart_grab) == 3)
				lua_helper::locals::set_int(FMC, fm_mission_controller_cart_grab, 4);
			else if (lua_helper::locals::get_int(FMC, fm_mission_controller_cart_grab) == 4)
				lua_helper::locals::set_float(FMC, fm_mission_controller_cart_grab + fm_mission_controller_cart_grab_speed, 2);
		}
		// https://github.com/YimMenu-Lua/FullSnacks/blob/main/FullSnacks.lua
		inline void increase_nightClub_popularity()
		{
			lua_helper::stats::set_int("MPX_CLUB_POPULARITY", 1000);
		}
		// see https://github.com/YimMenu-Lua/DailyCollectibles or https://github.com/YimMenu-Lua/MiniGameHack
		inline void set_stash_house_safecode()
		{
			auto stash_house_local_two = 119;
			for (int i = 0; i <= 2; i++)
			{
				auto safe_combination = lua_helper::locals::get_int("fm_content_stash_house", stash_house_local_two + 22 + (1 + (i * 2)) + 1);
				lua_helper::locals::set_float("fm_content_stash_house", stash_house_local_two + 22 + (1 + (i * 2)), safe_combination);
			}
		}
		// https://github.com/YimMenu-Lua/MiniGameHack
		inline void bypass_fingerprint_casino()
		{
			static constexpr auto local_H3_hack_1 = 53019;
			auto local_H3_hack_1_v                = lua_helper::locals::get_int(FMC, local_H3_hack_1);
			if ((local_H3_hack_1_v & (1 << 0)) == 0)
			{
				local_H3_hack_1_v = local_H3_hack_1_v ^ (1 << 0);
				lua_helper::locals::set_int(FMC, local_H3_hack_1, local_H3_hack_1_v);
			}
		}
		// https://github.com/YimMenu-Lua/MiniGameHack
		inline void bypass_keypad_casino()
		{
			constexpr auto local_H3_hack_2 = 54085;
			auto local_H3_hack_2_v         = lua_helper::locals::get_int(FMC, local_H3_hack_2);
			if ((local_H3_hack_2_v & (1 << 0)) == 0)
			{
				local_H3_hack_2_v = local_H3_hack_2_v ^ (1 << 0);
				lua_helper::locals::set_int(FMC, local_H3_hack_2, local_H3_hack_2_v);
			}
		}
		// https://github.com/YimMenu-Lua/MiniGameHack
		inline void casino_bypass_drill()
		{
			lua_helper::locals::set_int(FMC, 10109 + 2, 8);
		}
		// https://github.com/YimMenu-Lua/MiniGameHack
		inline void bypass_fingerprint_cayo()
		{
			constexpr int local_H4_hack = 24880;
			auto local_H4_hack_v        = lua_helper::locals::get_int(FMC20, local_H4_hack);
			if ((local_H4_hack_v & (1 << 0)) == 0)
			{
				local_H4_hack_v = local_H4_hack_v ^ (1 << 0);
				lua_helper::locals::set_int(FMC20, local_H4_hack, local_H4_hack_v);
			}
		}
		// https://github.com/YimMenu-Lua/MiniGameHack
		inline void bypass_plasma_cutter()
		{
			lua_helper::locals::set_float(FMC20, 30939 + 3, 100);
		}
		// https://github.com/YimMenu-Lua/DailyCollectibles
		inline void tp_to_g_cache_coords()
		{
			auto dead_drop_coords = [](int area, int location) -> std::optional<Vector3> {
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
			};

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
		// https://github.com/Deadlineem/Extras-Addon-for-YimMenu
		// no pv should be out, Gift Veh to player and ask him to replace the car in the garage (garage must be full)
		// ask him to add insurance afterwards he gets the car
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
		// https://github.com/L7NEG/Ultimate-Menu/blob/main/Kiddions/Ultimate_Menu_V2.1_1.68.lua SexChanger
		inline void allow_sex_change(bool is_sex_change_allowed)
		{
			lua_helper::stats::set_int("MPX_ALLOW_GENDER_CHANGE", is_sex_change_allowed ? 52 : 0);
		}
		// https://github.com/YimMenu-Lua/MiniGameHack
		inline void bypass_voltlab()
		{
			if (SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH(rage::joaat(FMC20)) != 0)
			{
				lua_helper::locals::set_int(FMC20, 1723, lua_helper::locals::get_int(FMC20, 1724));
				lua_helper::locals::set_int(FMC20, 1725, 3);
			}
			if (SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH(rage::joaat(FMCIH)) != 0)
			{
				lua_helper::locals::set_int(FMCIH, 766, lua_helper::locals::get_int(FMCIH, 767));
				lua_helper::locals::set_int(FMCIH, 768, 3);
			}
			if (SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH(rage::joaat(FMCVRP)) != 0)
			{
				lua_helper::locals::set_int(FMCVRP, 547, lua_helper::locals::get_int(FMCVRP, 548));
				lua_helper::locals::set_int(FMCVRP, 549, 3);
			}
			if (SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH(rage::joaat(AMACM)) != 0)
			{
				lua_helper::locals::set_int(AMACM, 455, lua_helper::locals::get_int(AMACM, 456));
				lua_helper::locals::set_int(AMACM, 457, 3);
			}
		}
		// https://github.com/YimMenu-Lua/Casino-Pacino/blob/main/CasinoPacino.lua
		inline int get_casino_target()
		{
			return lua_helper::stats::get_int("MPX_H3OPT_TARGET");
		}
		// https://github.com/YimMenu-Lua/Casino-Pacino/blob/main/CasinoPacino.lua
		inline void set_casino_target(int target)
		{
			lua_helper::stats::set_int("MPX_H3OPT_TARGET", target);
		}

		// https://github.com/SilentSal0/Silent-Night/blob/main/Silent%20Night%20v1.68.lua
		inline int get_cayo_target()
		{
			return lua_helper::stats::get_int("MPX_H4CNF_TARGET");
		}
		// https://github.com/SilentSal0/Silent-Night/blob/main/Silent%20Night%20v1.68.lua
		inline void set_cayo_target(int target)
		{
			lua_helper::stats::set_int("MPX_H4CNF_TARGET", target);
		}
		// https://github.com/SilentSal0/Silent-Night/blob/main/Silent%20Night%20v1.68.lua
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
		// https://github.com/SilentSal0/Silent-Night/blob/main/Silent%20Night%20v1.68.lua
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
		// https://github.com/SilentSal0/Silent-Night/blob/main/Silent%20Night%20v1.68.lua
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
		// https://github.com/YimMenu-Lua/GunVan
		inline void tp_to_gun_van()
		{
			constexpr float locs[][3] = {
			    {-29.532, 6435.136, 31.162},
			    {1705.214, 4819.167, 41.75},
			    {1795.522, 3899.753, 33.869},
			    {1335.536, 2758.746, 51.099},
			    {795.583, 1210.78, 338.962},
			    {-3192.67, 1077.205, 20.594},
			    {-789.719, 5400.921, 33.915},
			    {-24.384, 3048.167, 40.703},
			    {2666.786, 1469.324, 24.237},
			    {-1454.966, 2667.503, 3.2},
			    {2340.418, 3054.188, 47.888},
			    {1509.183, -2146.795, 76.853},
			    {1137.404, -1358.654, 34.322},
			    {-57.208, -2658.793, 5.737},
			    {1905.017, 565.222, 175.558},
			    {974.484, -1718.798, 30.296},
			    {779.077, -3266.297, 5.719},
			    {-587.728, -1637.208, 19.611},
			    {733.99, -736.803, 26.165},
			    {-1694.632, -454.082, 40.712},
			    {-1330.726, -1163.948, 4.313},
			    {-496.618, 40.231, 52.316},
			    {275.527, 66.509, 94.108},
			    {260.928, -763.35, 30.559},
			    {-478.025, -741.45, 30.299},
			    {894.94, 3603.911, 32.56},
			    {-2166.511, 4289.503, 48.733},
			    {1465.633, 6553.67, 13.771},
			    {1101.032, -335.172, 66.944},
			    {149.683, -1655.674, 29.028},
			};

			if (auto loc = lua_helper::globals::get_int(2652592 + 2671); loc != -1)
				teleport::to_coords(Vector3(locs[loc][0], locs[loc][1], locs[loc][2]));
		}
	}
}