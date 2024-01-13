#include "core/enums.hpp"
#include "core/scr_globals.hpp"
#include "gta/net_array.hpp"
#include "gta_util.hpp"
#include "pointers.hpp"

#include <imgui.h>

static constexpr char transition_states[][48] = {"TRANSITION_STATE_EMPTY", "Singleplayer Swoop Up", "Multiplayer Swoop Up", "Creator Swoop Up", "Pre-HUD Checks", "Wait HUD Exit", "Wait For Summon", "Singleplayer Swoop Down", "Multiplayer Swoop Down", "Cancel Joining", "Retry Loading", "Retry Loading Slot 1", "Retry Loading Slot 2", "Retry Loading Slot 3", "Retry Loading Slot 4", "Wait On Invite", "Prejoining Freemode Session Checks", "Look For Fresh Join Freemode", "Look To Join Another Session Freemode", "Confirm Freemode Session Joining", "Wait Join Freemode Session", "Creation Enter Session", "Pre-Freemode Launch Script", "Freemode Teamfull Check", "Start Freemode Launch Script", "Freemode Transition Create Player", "Is Freemode And Transition Ready", "Freemode Swoop Down", "Post Bink Video Warp", "Freemode Final Setup Player", "Move Freemode To Running State", "Freemode How To Terminate", "Start Creator Pre-Launch Script Check", "Start Creator Launch Script", "Creator Transition Create Player", "Is Creator And Transition Ready", "Creator Swoop Down", "Creator Final Setup Player", "Move Creator To Running State", "Prejoining Testbed Session Checks", "Look For Fresh Join Testbed", "Look For Fresh Host Testbed", "Look To Join Another Session Testbed", "Look To Host Session Testbed", "Confirm Testbed Session Joining", "Wait Join Testbed Session", "Start Testbed Launch Script", "Testbed Transition Create Player", "Is Testbed And Transition Ready", "Testbed Swoop Down", "Testbed Final Setup Player", "Move Testbed To Running State", "Testbed How To Terminate", "Quit Current Session Prompt", "Wait For Transition Session To Setup", "Terminate Singleplayer", "Wait Terminate Singleplayer", "Kick Terminate Session", "Terminate Session", "Wait Terminate Session", "Terminate Session And Hold", "Terminate Session And Move Into Holding State", "Team Swapping Checks", "Return To Singleplayer", "Wait For Singleplayer To Start", "Waiting For External Termination Call", "Terminate Maintransition", "Wait For Dirty Load Confirm", "DLC Intro Bink", "Spawn Into Personal Vehicle"};

namespace big
{
	inline void view_debug_info()
	{
		if (ImGui::BeginTabItem("Info"))
		{
			ImGui::Text(std::format("Game Version: {}", g_pointers->m_gta.m_game_version).c_str());
			ImGui::Text(std::format("Online Version: {}", g_pointers->m_gta.m_online_version).c_str());

			const auto state          = *scr_globals::transition_state.as<eTransitionState*>();
			const char* state_text    = transition_states[(int)state];
			rage::scrThread* freemode = nullptr;

			if (state == eTransitionState::TRANSITION_STATE_FM_TRANSITION_CREATE_PLAYER
			    && (freemode = gta_util::find_script_thread(RAGE_JOAAT("freemode")), freemode && freemode->m_net_component))
			{
				int num_array_handlers{};
				int received_array_handlers{};

				while (auto handler =
				           g_pointers->m_gta.m_get_host_array_handler_by_index((CGameScriptHandlerNetComponent*)freemode->m_net_component, num_array_handlers++))
					if (handler->m_flags & 1)
						received_array_handlers++;

				if (num_array_handlers == 0)
					num_array_handlers = 1;

				float percent = round((static_cast<float>(received_array_handlers) / num_array_handlers) * 100);

				ImGui::Text(std::format("Transition State: Host Broadcast Data ({})", percent).c_str());
			}
			else
				ImGui::Text(std::format("Transition State: {}", state_text).c_str());

			ImGui::EndTabItem();
		}
	}
}
