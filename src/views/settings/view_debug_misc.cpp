#include "gui/components/components.hpp"
#include "natives.hpp"

namespace big
{
	inline void view_debug_misc()
	{
		if (ImGui::BeginTabItem("Misc"))
		{
			static bool imGui_demo = false;

			ImGui::Spacing();

			components::button("Skip Cutscene", [] {
				CUTSCENE::STOP_CUTSCENE_IMMEDIATELY();
			});

			ImGui::Spacing();

			components::button("Network Bail", [] {
				NETWORK::NETWORK_BAIL(16, 0, 0);
			});

			ImGui::Spacing();

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
