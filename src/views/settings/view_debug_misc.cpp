#include "gui/components/components.hpp"
#include "natives.hpp"
#include "util/local_player.hpp"
#include "util/lua_script.cpp"
#include "util/pools.hpp"

namespace big
{
	inline void view_debug_misc()
	{
		if (ImGui::BeginTabItem("Misc"))
		{
			static bool imGui_demo = false;

			ImGui::Spacing();

			components::button("Die", [] {
				ENTITY::SET_ENTITY_HEALTH(self::ped, 0, 0);
			});
			ImGui::SameLine();
			components::button("Toggle Radar", [] {
				HUD::DISPLAY_RADAR(HUD::IS_RADAR_HIDDEN());
			});

			ImGui::Spacing();

			components::button("Skip Cutscene", [] {
				CUTSCENE::STOP_CUTSCENE_IMMEDIATELY();
			});
			ImGui::SameLine();
			components::button("Clear Attachments", [] {
				for (auto obj : pools::get_all_props())
				{
					auto object = g_pointers->m_gta.m_ptr_to_handle(obj);
					if (!object)
						break;

					if (!ENTITY::IS_ENTITY_ATTACHED_TO_ENTITY(self::ped, object))
						continue;

					ENTITY::DELETE_ENTITY(&object);
				}
			});

			ImGui::Spacing();

			components::button("CLEAR_PED_TASKS", [] {
				TASK::CLEAR_PED_TASKS(self::ped);
			});
			ImGui::SameLine();
			components::button("CLEAR_PED_TASKS_IMMEDIATELY", [] {
				TASK::CLEAR_PED_TASKS_IMMEDIATELY(self::ped);
			});

			ImGui::Spacing();

			components::button("Log no of seats", [] {
				if (self::veh)
					LOG(WARNING) << "Debug: No of seats in current vehicle: " << (VEHICLE::GET_VEHICLE_MAX_NUMBER_OF_PASSENGERS(self::veh) + 1);
			});

			ImGui::Spacing();

			components::button("Network Bail", [] {
				NETWORK::NETWORK_BAIL(16, 0, 0);
			});
			ImGui::SameLine();
			components::button("Rage Quit", [] {
				exit(0);
			});

			ImGui::Spacing();

			ImGui::Checkbox("ImGui Demo", &imGui_demo);

			if (imGui_demo)
				ImGui::ShowDemoWindow(&imGui_demo);

			ImGui::EndTabItem();
		}
	}
}
