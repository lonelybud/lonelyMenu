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

			// components::sub_title("Experiments");
			// {
			// }

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

				components::button("Remove Black Screen", [] {
					CAM::DO_SCREEN_FADE_IN(0);
					PLAYER::SET_PLAYER_CONTROL(self::id, true, 0);
					ENTITY::FREEZE_ENTITY_POSITION(self::ped, false);
					MISC::FORCE_GAME_STATE_PLAYING();
					if (self::veh == 0)
						TASK::CLEAR_PED_TASKS_IMMEDIATELY(self::ped);
					HUD::DISPLAY_RADAR(true);
					HUD::DISPLAY_HUD(true);
				});

				components::button("Play sound (Locally)", [] {
					AUDIO::PLAY_SOUND_FROM_ENTITY(1, "SELECT", self::ped, "HUD_FRONTEND_DEFAULT_SOUNDSET", 0, 0);
				});
			}

			components::sub_title("Log");
			{
				components::button("Is Activity Session?", [] {
					LOG(VERBOSE) << "Is Activity Session?: " << NETWORK::NETWORK_IS_ACTIVITY_SESSION();
				});

				ImGui::BeginGroup();
				{
					ImGui::Checkbox("Log packets", &g_debug.log_packets);
					ImGui::Checkbox("Log script events", &g_debug.log_script_events);
					ImGui::Checkbox("Log Clones", &g_debug.log_clones);
				}
				ImGui::EndGroup();
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
