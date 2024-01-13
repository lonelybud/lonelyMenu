#include "core/data/misc.hpp"
#include "core/scr_globals.hpp"
#include "util/entity.hpp"
#include "util/lua_script.hpp"
#include "util/ped.hpp"
#include "util/teleport.hpp"
#include "views/view.hpp"

namespace big
{
	// https://github.com/SilentSal0/Silent-Night
	constexpr int CPFHl   = 24333;      // cayo perico fingerprint hack local
	constexpr int CPPCCl  = 30357 + 3;  // cayo perico plasma cutter cut local ("DLC_H4_anims_glass_cutter_Sounds")
	constexpr int CPSTCl  = 29118;      // cayo perico sewer tunnel cut local
	constexpr int DCFHl   = 52985;      // diamond casino fingerprint hack local
	constexpr int DCKHl   = 54047;      // diamond casino keypad hack local
	constexpr int DCDVDl1 = 10107 + 7;  // diamond casino drill vault door local 1
	constexpr int DCDVDl2 = 10107 + 37; // diamond casino drill vault door local 2
	auto FMC              = "fm_mission_controller";
	auto FMC20            = "fm_mission_controller_2020";

	// lua_script::locals::get_int("fm_mission_controller", fm_mission_controller_cart_grab)
	// lua_script::locals::set_int("fm_mission_controller", fm_mission_controller_cart_grab, 4);

	void view::misc()
	{
		ImGui::SeparatorText("World");
		{
			components::command_checkbox<"timeoverride">();

			if (g_misc.custom_time.override_time)
			{
				ImGui::PushItemWidth(150);
				ImGui::SliderInt("Hour", &g_misc.custom_time.hour, 0, 23);
				ImGui::SliderInt("Minute", &g_misc.custom_time.minute, 0, 59);
				ImGui::SliderInt("Second", &g_misc.custom_time.second, 0, 59);
				ImGui::PopItemWidth();
			}

			components::button("Increase NightClub Popularity", [] {
				lua_script::stats::set_int("MPX_CLUB_POPULARITY", 1000);
			});
		}

		ImGui::SeparatorText("Ped");
		{
			components::command_checkbox<"pedsignore">();

			ImGui::Spacing();

			components::button("Kill All Enemies", [] {
				for (auto ped : entity::get_entities(false, true))
					if (!PED::IS_PED_A_PLAYER(ped))
						if (auto relation = PED::GET_RELATIONSHIP_BETWEEN_PEDS(ped, self::ped); relation == 4 || relation == 5 || relation == 3)
							ped::kill_ped(ped);
			});
			ImGui::SameLine();
			components::button("Kill Police", [] {
				if (g_local_player->m_player_info->m_is_wanted)
					for (auto ped : entity::get_entities(false, true))
						if (!PED::IS_PED_A_PLAYER(ped))
							if (auto relation = PED::GET_RELATIONSHIP_BETWEEN_PEDS(ped, self::ped); relation == 255)
								ped::kill_ped(ped);
			});
		}

		ImGui::SeparatorText("Other");
		{
			ImGui::Checkbox("Request Control", &g_misc.request_control);
			ImGui::Checkbox("Notify attack of player on friend", &g_misc.notify_friend_killed);

			ImGui::Spacing();

			components::button("Die", [] {
				ENTITY::SET_ENTITY_HEALTH(self::ped, 0, 0, 0);
			});
			ImGui::SameLine();
			components::button("Skip Cutscene", [] {
				CUTSCENE::STOP_CUTSCENE_IMMEDIATELY();
			});
			ImGui::SameLine();
			components::button("Force passive mode (30 secs)", [] {
				*scr_globals::passive.as<PBOOL>() = TRUE;
			});

			ImGui::Spacing();

			components::button("Clear Tasks", [] {
				TASK::CLEAR_PED_TASKS(self::ped);
			});
			ImGui::SameLine();
			components::button("Clear Tasks Immediately", [] {
				TASK::CLEAR_PED_TASKS_IMMEDIATELY(self::ped);
			});

			ImGui::Spacing();

			components::button("Toggle Radar", [] {
				HUD::DISPLAY_RADAR(HUD::IS_RADAR_HIDDEN());
			});

			components::button("Remove all sticky bombs", [] {
				Entity entity = self::veh ? self::veh : self::ped;
				NETWORK::REMOVE_ALL_STICKY_BOMBS_FROM_ENTITY(entity, self::ped);
			});
			ImGui::SameLine();
			components::button("Wet yourself", [] {
				PED::SET_PED_WETNESS_HEIGHT(self::ped, 1);
			});
		}

		ImGui::SeparatorText("Daily Collectables");
		{
			static std::string combination_retn;
			components::button("Show Stash House safecodes", [] {
				combination_retn = "";
				for (int i = 0; i <= 2; i++)
					if (i == 2)
						combination_retn += std::to_string(lua_script::locals::get_int("fm_content_stash_house", 117 + 22 + (1 + (i * 2)) + 1));
					else
						combination_retn += std::to_string(lua_script::locals::get_int("fm_content_stash_house", 117 + 22 + (1 + (i * 2)) + 1)) + "-";
			});
			ImGui::SameLine();
			ImGui::Text(combination_retn.c_str());

			components::button("TP to G'cache object", [] {
				if (*g_pointers->m_gta.m_script_globals && **g_pointers->m_gta.m_script_globals)
					if (auto gs_cache_box_entity = *scr_globals::pickups.at(605).as<Entity*>(); gs_cache_box_entity != 0)
						teleport::tp_on_top(gs_cache_box_entity);
			});
		}

		ImGui::SeparatorText("Casino Heist");
		{
			components::command_checkbox<"casino_cart_item_auto_grab">();

			components::button("Bypass Fingerprint##casino", [] {
				if (lua_script::locals::get_int(FMC, DCFHl) == 4)
					lua_script::locals::set_int(FMC, DCFHl, 5);
			});
			ImGui::SameLine();
			components::button("Bypass Keypad", [] {
				if (lua_script::locals::get_int(FMC, DCKHl) != 4)
					lua_script::locals::set_int(FMC, DCKHl, 5);
			});
			ImGui::SameLine();
			components::button("Bypass Drill", [] {
				lua_script::locals::set_int(FMC, DCDVDl1, lua_script::locals::get_int(FMC, DCDVDl2));
			});
		}

		ImGui::SeparatorText("Cayo Heist");
		{
			components::button("Bypass Fingerprint##cayo", [] {
				if (lua_script::locals::get_int(FMC20, CPFHl) == 4)
					lua_script::locals::set_int(FMC20, CPFHl, 5);
			});
			ImGui::SameLine();
			components::button("Bypass Plasma Cutter", [] {
				lua_script::locals::set_float(FMC20, CPPCCl, 100);
			});
			ImGui::SameLine();
			components::button("Bypass Sewer Tunnel", [] {
				if (auto v = lua_script::locals::get_int(FMC20, CPSTCl); v >= 3 || v <= 6)
					lua_script::locals::set_int(FMC20, CPSTCl, 6);
			});
		}
	}
}
