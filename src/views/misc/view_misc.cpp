#include "core/data/lua.hpp"
#include "core/data/misc.hpp"
#include "core/enums.hpp"
#include "core/scr_globals.hpp"
#include "fiber_pool.hpp"
#include "util/entity.hpp"
#include "util/mobile.hpp"
#include "util/ped.hpp"
#include "util/session.hpp"
#include "views/view.hpp"

#include <script/globals/GPBD_FM.hpp>

namespace big
{
	static inline void render_time()
	{
		components::sub_title("Time");
		components::command_checkbox<"timeoverride">();
		if (g_misc.custom_time.override_time)
		{
			ImGui::PushItemWidth(150);
			ImGui::SliderInt("Hour", &g_misc.custom_time.hour, 0, 23);
			ImGui::SliderInt("Minute", &g_misc.custom_time.minute, 0, 59);
			ImGui::SliderInt("Second", &g_misc.custom_time.second, 0, 59);
			ImGui::PopItemWidth();
		}
	}

	static inline void game()
	{
		components::sub_title("Game");


		components::button("Add thunder", [] {
			session::set_fm_event_index(9);
			session::set_fm_event_index(10);
			session::set_fm_event_index(11);
		});
		ImGui::SameLine();
		components::button("Remove thunder", [] {
			session::clear_fm_event_index(9);
			session::clear_fm_event_index(10);
			session::clear_fm_event_index(11);
		});

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

	static inline void others()
	{
		components::sub_title("Others");

		ImGui::Checkbox("Request Control of Entity", &g_misc.request_control);
		ImGui::Checkbox("Notify friend killed", &g_misc.notify_friend_killed);

		ImGui::Spacing();
		
		components::button("Start New Public", [] {
			session::join_type(eSessionType::NEW_PUBLIC);
		});
		ImGui::SameLine();
		components::button("Leave Online", [] {
			session::join_type(eSessionType::LEAVE_ONLINE);
		});

		ImGui::Spacing();
		
		static bool is_sex_change_allowed = false;
		if (ImGui::Checkbox("Allow gender change", &is_sex_change_allowed))
			g_fiber_pool->queue_job([] {
				lua_scripts::allow_sex_change(is_sex_change_allowed);
			});
		static bool is_chang_app_cooldwn_enab = false;
		if (ImGui::Checkbox("Disable Change Appearance Cooldown", &is_chang_app_cooldwn_enab))
			g_fiber_pool->queue_job([] {
				lua_scripts::change_appearance_cooldown(is_chang_app_cooldwn_enab);
			});
	}

	static inline void service_vehicles()
	{
		components::sub_title("Request Service Vehicles");

		components::button("Avenger", [] {
			mobile::services::request_avenger();
		});
		ImGui::SameLine();
		components::button("Kosatka", [] {
			mobile::services::request_kosatka();
		});
		ImGui::SameLine();
		components::button("M.O.C", [] {
			mobile::services::request_mobile_operations_center();
		});

		components::button("Terrorbyte", [] {
			mobile::services::request_terrorbyte();
		});
		ImGui::SameLine();
		components::button("Acid Lab", [] {
			mobile::services::request_acidlab();
		});
		ImGui::SameLine();
		components::button("Acid Bike", [] {
			mobile::services::request_acidlab_bike();
		});
	}

	static inline void _self()
	{
		components::sub_title("Self");

		components::button("Die", [] {
			ENTITY::SET_ENTITY_HEALTH(self::ped, 0, 0, 0);
		});
		ImGui::SameLine();
		components::button("Skip Cutscene", [] {
			CUTSCENE::STOP_CUTSCENE_IMMEDIATELY();
		});
		ImGui::SameLine();
		components::button("Force passive", [] {
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
		components::button("Remove stickies", [] {
			Entity entity = self::veh ? self::veh : self::ped;
			NETWORK::REMOVE_ALL_STICKY_BOMBS_FROM_ENTITY(entity, 0);
		});
		ImGui::SameLine();
		components::button("Get Wet", [] {
			PED::SET_PED_WETNESS_HEIGHT(self::ped, 1);
			PED::SET_PED_WETNESS_ENABLED_THIS_FRAME(self::ped);
		});
	}

	static inline void properties()
	{
		components::sub_title("Properties");

		if (*g_pointers->m_gta.m_script_globals && **g_pointers->m_gta.m_script_globals)
			if (auto gpbd_fm_1 = scr_globals::gpbd_fm_1.as<GPBD_FM*>(); gpbd_fm_1)
			{
				auto& v = gpbd_fm_1->Entries[self::id];
				ImGui::Text("Nightclub popularity: %f", v.PropertyData.NightclubData.Popularity);
				ImGui::Text("Nightclub Money: %d", v.PropertyData.NightclubData.SafeCashValue);
				ImGui::Text("Arcade Money: %d", v.PropertyData.ArcadeData.SafeCashValue);

				components::button("Increase NightClub Popularity", [] {
					lua_scripts::increase_nightClub_popularity();
				});
			}
	}

	static inline void _vehicles()
	{
		components::sub_title("Vehicles");

		components::button("Set veh no longer needed", [] {
			if (self::veh)
			{
				auto veh = self::veh;
				ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&veh);
			}
		});
		components::button("Set as stolen", [] {
			if (self::veh)
				VEHICLE::SET_VEHICLE_IS_STOLEN(self::veh, TRUE);
		});
		components::button("Reset veh mod kit", [] {
			if (self::veh)
				VEHICLE::SET_VEHICLE_MOD_KIT(self::veh, 65535);
		});
		components::button("Unlock last vehicle", [] {
			if (auto veh = PLAYER::GET_PLAYERS_LAST_VEHICLE(); veh && entity::take_control_of(veh))
				VEHICLE::SET_VEHICLE_DOORS_LOCKED(veh, (int)eVehicleLockState::VEHICLELOCK_UNLOCKED);
		});
		components::button("Set God", [] {
			if (self::veh && entity::take_control_of(self::veh))
				ENTITY::SET_ENTITY_INVINCIBLE(self::veh, TRUE);
		});
		ImGui::SameLine();
		components::button("Unset God", [] {
			if (self::veh && entity::take_control_of(self::veh))
				ENTITY::SET_ENTITY_INVINCIBLE(self::veh, FALSE);
		});
	}

	static inline void daily_collectables()
	{
		components::sub_title("Daily Collectables");

		static std::string combination_retn;
		components::button("Show Stash House safecodes", [] {
			combination_retn = lua_scripts::get_stash_house_safecode();
		});
		ImGui::SameLine();
		ImGui::Text(combination_retn.c_str());

		components::button("TP to G'cache object", [] {
			lua_scripts::tp_to_g_cache_coords();
		});
	}

	static inline void casino_heist()
	{
		components::sub_title("Casino Heist");

		static int casino_target = 0;
		ImGui::SetNextItemWidth(100);
		if (ImGui::Combo("Target###casino", &casino_target, lua_scripts::casino_targets, IM_ARRAYSIZE(lua_scripts::casino_targets)))
			g_fiber_pool->queue_job([=] {
				lua_scripts::set_casino_target(casino_target);
			});
		ImGui::SameLine();
		components::button("Refresh##ctrefresh", [] {
			casino_target = lua_scripts::get_casino_target();
		});

		components::command_checkbox<"casino_cart_item_auto_grab">();

		components::button("BP Finger##casino", [] {
			lua_scripts::bypass_fingerprint_casino();
		});
		ImGui::SameLine();
		components::button("BP Keypad##casino", [] {
			lua_scripts::bypass_keypad_casino();
		});
		ImGui::SameLine();
		components::button("BP Drill", [] {
			lua_scripts::bypass_drill();
		});
	}

	static inline void cayo_heist()
	{
		components::sub_title("Cayo Heist");

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

		components::button("Log Cayo Details", [] {
			lua_scripts::log_cayo_details();
		});

		components::button("Set second targ", [] {
			lua_scripts::set_compound_and_island_targets();
		});
		ImGui::SameLine();
		components::button("Scope second targ", [] {
			lua_scripts::scope_compound_and_island_targets();
		});

		components::button("BP Finger##cayo", [] {
			lua_scripts::bypass_fingerprint_cayo();
		});
		ImGui::SameLine();
		components::button("BP Plasma", [] {
			lua_scripts::bypass_plasma_cutter();
		});
	}

	void view::misc()
	{
		ImGui::BeginGroup();
		{
			render_time();
			components::ver_space();
			game();
			components::ver_space();
			others();
			components::ver_space();
			service_vehicles();
		}
		ImGui::EndGroup();
		components::hor_space();
		ImGui::BeginGroup();
		{
			_self(); // name_self to avoid calling same
			components::ver_space();
			properties();
			components::ver_space();
			_vehicles();
		}
		ImGui::EndGroup();
		components::hor_space();
		ImGui::BeginGroup();
		{
			daily_collectables();
			components::ver_space();
			casino_heist();
			components::ver_space();
			cayo_heist();
		}
		ImGui::EndGroup();
	}
}
