#include "core/data/debug.hpp"
#include "core/scr_globals.hpp"
#include "gui/components/components.hpp"
#include "natives.hpp"
#include "util/entity.hpp"
#include "util/local_player.hpp"
#include "util/lua_script.cpp"
#include "util/ped.hpp"
#include "util/pools.hpp"

namespace big
{
	inline void view_debug_misc()
	{
		if (ImGui::BeginTabItem("Misc"))
		{
			ImGui::Spacing();

			components::sub_title("Self");
			{
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
			}

			components::sub_title("World");
			{
				components::button("Kill All Enemies", [] {
					for (auto ped : entity::get_entities(false, true))
						if (!PED::IS_PED_A_PLAYER(ped))
							if (auto relation = PED::GET_RELATIONSHIP_BETWEEN_PEDS(ped, self::ped); relation == 4 || relation == 5 || relation == 3)
								ped::kill_ped(ped);
				});
			}

			components::sub_title("Tasks");
			{
				components::button("Clear Tasks", [] {
					TASK::CLEAR_PED_TASKS(self::ped);
				});
				ImGui::SameLine();
				components::button("Clear Tasks Immediately", [] {
					TASK::CLEAR_PED_TASKS_IMMEDIATELY(self::ped);
				});
			}

			components::sub_title("Screen");
			{
				components::button("Toggle Radar", [] {
					HUD::DISPLAY_RADAR(HUD::IS_RADAR_HIDDEN());
				});
			}

			components::sub_title("Experiments");
			{
				ImGui::Checkbox("Auto Grab Money etc", &g_debug.fm_mission_controller_cart_grab);
				ImGui::Checkbox("Request Control", &g_debug.request_control);
			}

			components::sub_title("Game");
			{
				components::button("Network Bail", [] {
					NETWORK::NETWORK_BAIL(16, 0, 0);
				});
				ImGui::SameLine();
				components::button("Rage Quit", [] {
					exit(0);
				});
			}

			// ImGui::Spacing();
			// static bool imGui_demo = false;
			// ImGui::Checkbox("ImGui Demo", &imGui_demo);
			// if (imGui_demo)
			// 	ImGui::ShowDemoWindow(&imGui_demo);

			ImGui::EndTabItem();
		}
	}
}
