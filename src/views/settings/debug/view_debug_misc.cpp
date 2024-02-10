#include "core/data/debug.hpp"
#include "gui/components/components.hpp"
#include "natives.hpp"

namespace big
{
	inline void view_debug_misc()
	{
		if (ImGui::BeginTabItem("Misc"))
		{
			ImGui::Spacing();

			components::sub_title("Experiments");
			{
				ImGui::Checkbox("Disable Population Load Balancing", &g_debug.disable_population_load_balancing);
				ImGui::Checkbox("Freemode Hook G_N_O_T_R_T_S_W_T_H", &g_debug.freemode_hook_G_N_O_T_R_T_S_W_T_H);
				ImGui::Checkbox("Log Explosion Event", &g_debug.log_explosion_event);
			}

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

				components::button("End tutorial", [] {
					NETWORK::NETWORK_END_TUTORIAL_SESSION();
				});

				components::button("Load recent SP save", [] {
					NETWORK::SHUTDOWN_AND_LOAD_MOST_RECENT_SAVE();
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
