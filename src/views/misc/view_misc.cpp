#include "core/data/lua.hpp"
#include "core/data/misc.hpp"
#include "core/enums.hpp"
#include "core/scr_globals.hpp"
#include "util/entity.hpp"
#include "util/ped.hpp"
#include "util/teleport.hpp"
#include "views/view.hpp"

namespace big
{
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
				lua_scripts::increase_nightClub_popularity();
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
			ImGui::SameLine();
			components::button("Remove all sticky bombs", [] {
				Entity entity = self::veh ? self::veh : self::ped;
				NETWORK::REMOVE_ALL_STICKY_BOMBS_FROM_ENTITY(entity, self::ped);
			});
			ImGui::SameLine();
			components::button("Wet yourself", [] {
				PED::SET_PED_WETNESS_HEIGHT(self::ped, 1);
			});

			ImGui::Spacing();
			components::button("Start New Public Session", [] {
				if (SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH(RAGE_JOAAT("maintransition")) != 0 || STREAMING::IS_PLAYER_SWITCH_IN_PROGRESS())
				{
					g_notification_service->push_error("Failed", "Player switch in progress, wait a bit.");
					return;
				}

				SCRIPT::REQUEST_SCRIPT_WITH_NAME_HASH(RAGE_JOAAT("pausemenu_multiplayer"));
				while (!SCRIPT::HAS_SCRIPT_WITH_NAME_HASH_LOADED(RAGE_JOAAT("pausemenu_multiplayer")))
					script::get_current()->yield();

				*scr_globals::session2.as<int*>() = (int)eSessionType::NEW_PUBLIC;
				*scr_globals::session.as<int*>()  = 1;
				script::get_current()->yield(200ms);
				*scr_globals::session.as<int*>() = 0;

				SCRIPT::SET_SCRIPT_WITH_NAME_HASH_AS_NO_LONGER_NEEDED(RAGE_JOAAT("pausemenu_multiplayer"));
			});
		}

		ImGui::SeparatorText("Daily Collectables");
		{
			static std::string combination_retn;
			components::button("Show Stash House safecodes", [] {
				combination_retn = lua_scripts::get_stash_house_safecode();
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
			static int casino_target = 0;
			ImGui::SetNextItemWidth(200);
			if (ImGui::Combo("Target###casino", &casino_target, lua_scripts::casino_targets, IM_ARRAYSIZE(lua_scripts::casino_targets)))
				g_fiber_pool->queue_job([=] {
					lua_scripts::set_casino_target(casino_target);
				});
			ImGui::SameLine();
			components::button("Refresh##ctrefresh", [] {
				casino_target = lua_scripts::get_casino_target();
			});

			components::command_checkbox<"casino_cart_item_auto_grab">();

			components::button("Bypass Fingerprint##casino", [] {
				lua_scripts::bypass_fingerprint_casino();
			});
			ImGui::SameLine();
			components::button("Bypass Keypad", [] {
				lua_scripts::bypass_keypad_casino();
			});
			ImGui::SameLine();
			components::button("Bypass Drill", [] {
				lua_scripts::bypass_drill();
			});
		}

		ImGui::SeparatorText("Cayo Heist");
		{
			static int cayo_target = 0;
			ImGui::SetNextItemWidth(200);
			if (ImGui::Combo("Target###Cayo", &cayo_target, lua_scripts::cayo_targets, IM_ARRAYSIZE(lua_scripts::cayo_targets)))
				g_fiber_pool->queue_job([=] {
					lua_scripts::set_cayo_target(cayo_target);
				});
			ImGui::SameLine();
			components::button("Refresh##ctrefresh", [] {
				cayo_target = lua_scripts::get_cayo_target();
			});
			ImGui::SameLine();
			components::button("Log Cayo Details", [] {
				lua_scripts::log_cayo_details();
			});
			ImGui::SameLine();
			components::button("Set comp/island targets", [] {
				lua_scripts::set_compound_and_island_targets();
			});
			ImGui::SameLine();
			components::button("Scope comp/island targets", [] {
				lua_scripts::scope_compound_and_island_targets();
			});

			components::button("Bypass Fingerprint##cayo", [] {
				lua_scripts::bypass_fingerprint_cayo();
			});
			ImGui::SameLine();
			components::button("Bypass Plasma Cutter", [] {
				lua_scripts::bypass_plasma_cutter();
			});
			ImGui::SameLine();
			components::button("Bypass Sewer Tunnel", [] {
				lua_scripts::bypass_sewer_tunnel();
			});
		}
	}
}
