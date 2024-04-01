#include "core/data/debug.hpp"
#include "gui/components/components.hpp"
#include "natives.hpp"
#include "util/ped.hpp"

namespace big
{
	inline void view_debug_misc()
	{
		if (ImGui::BeginTabItem("Misc"))
		{
			ImGui::Spacing();

			components::sub_title("Experiments");
			{
				ImGui::Checkbox("Freemode Hook G_N_O_T_R_T_S_W_T_H", &g_debug.freemode_hook_G_N_O_T_R_T_S_W_T_H);
			}

			components::sub_title("Game");
			{
				ImGui::Checkbox("use_default_chat", &g_debug.use_default_chat);

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

			components::sub_title("Log");
			{
				components::button("Is Activity Session?", [] {
					LOG(VERBOSE) << "Is Activity Session?: " << NETWORK::NETWORK_IS_ACTIVITY_SESSION();
				});

				ImGui::Checkbox("Log Explosion Event", &g_debug.log_explosion_event);
				ImGui::Checkbox("Log packets", &g_debug.log_packets);
				ImGui::Checkbox("Log script events", &g_debug.log_script_events);
				ImGui::Checkbox("Log Clones", &g_debug.log_clones);
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
