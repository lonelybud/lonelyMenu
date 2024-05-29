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
				ImGui::Checkbox("Enable Objects Crash 2 Protection", &g_debug.enable_objects_crash_2);
			}

			components::sub_title("Game");
			{
				// ImGui::Checkbox("use_default_chat", &g_debug.use_default_chat);

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

				components::button("Play sound", [] {
					AUDIO::PLAY_SOUND_FROM_ENTITY(1, "SELECT", self::ped, "HUD_FRONTEND_DEFAULT_SOUNDSET", 0, 0);
				});
			}

			components::sub_title("Log");
			{
				if (components::button("Log Hello World"))
					LOG(VERBOSE) << "Hello World";

				components::button("Is Activity Session?", [] {
					LOG(VERBOSE) << "Is Activity Session?: " << NETWORK::NETWORK_IS_ACTIVITY_SESSION();
				});

				ImGui::Checkbox("Log packets", &g_debug.log_packets);
				ImGui::Checkbox("Log script events", &g_debug.log_script_events);
				ImGui::Checkbox("Log Clones", &g_debug.log_clones);
				ImGui::Spacing();
				ImGui::Checkbox("Log Vehicle Clones", &g_debug.log_vehicle_clones);
				ImGui::Checkbox("Log Ped Clones", &g_debug.log_ped_clones);
				ImGui::Checkbox("Log Explosion Event", &g_debug.log_explosion_event);
				ImGui::Checkbox("Log Ptfx Event", &g_debug.log_ptfx_event);
				ImGui::Checkbox("Log Sound Event", &g_debug.log_sound_event);
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
