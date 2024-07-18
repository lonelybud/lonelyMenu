#include "core/data/misc.hpp"
#include "core/enums.hpp"
#include "core/scr_globals.hpp"
#include "services/tunables/tunables_service.hpp"
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

	static inline void render_game()
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

	static inline void render_others()
	{
		components::sub_title("Others");

		ImGui::Checkbox("Notify friend killed", &g_misc.notify_friend_killed);
	}

	static inline void render_logs()
	{
		components::sub_title("Logs");

		ImGui::Checkbox("Log Vehicle Clones", &g_misc.log_vehicle_clones);
		ImGui::Checkbox("Log Ped Clones", &g_misc.log_ped_clones);
		ImGui::Checkbox("Log Object Clones", &g_misc.log_object_clones);
		ImGui::Spacing();
		ImGui::Checkbox("Log Explosion Event", &g_misc.log_explosion_event);
		ImGui::Checkbox("Log Ptfx Event", &g_misc.log_ptfx_event);
		ImGui::Checkbox("Log Sound Event", &g_misc.log_sound_event);
		ImGui::Spacing();
		ImGui::Checkbox("Log CPedHealthDataNode", &g_misc.log_CPedHealthDataNode);
		ImGui::Checkbox("Log CVehicleTaskDataNode", &g_misc.log_CVehicleTaskDataNode);
	}

	static inline void render_service_vehicles()
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

	static inline void render_self()
	{
		components::sub_title("Self");

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
	}

	static inline void render_properties()
	{
		components::sub_title("Properties");

		if (*g_pointers->m_gta.m_script_globals && **g_pointers->m_gta.m_script_globals)
			if (auto gpbd_fm_1 = scr_globals::gpbd_fm_1.as<GPBD_FM*>(); gpbd_fm_1)
			{
				auto& v = gpbd_fm_1->Entries[self::id];
				ImGui::Text("Nightclub popularity: %f", v.PropertyData.NightclubData.Popularity);
				ImGui::Text("Nightclub Money: %d", v.PropertyData.NightclubData.SafeCashValue);
				ImGui::Text("Arcade Money: %d", v.PropertyData.ArcadeData.SafeCashValue);
			}
	}

	static inline void render_vehicles()
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

	void view::misc()
	{
		ImGui::BeginGroup();
		{
			render_time();
			components::ver_space();
			render_game();
			components::ver_space();
			render_service_vehicles();
		}
		ImGui::EndGroup();
		components::hor_space();
		ImGui::BeginGroup();
		{
			render_self();
			components::ver_space();
			render_properties();
			components::ver_space();
			render_vehicles();
		}
		ImGui::EndGroup();
		components::hor_space();
		ImGui::BeginGroup();
		{
			render_others();
			components::ver_space();
			render_logs();
		}
		ImGui::EndGroup();
	}
}
