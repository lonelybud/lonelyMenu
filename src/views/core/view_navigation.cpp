#include "core/data/gui_info.hpp"
#include "core/settings/window.hpp"
#include "services/gui/gui_service.hpp"
#include "views/view.hpp"

namespace big
{
	void view::navigation()
	{
		ImGui::SetNextWindowPos({g_gui_info.nav_window_pos_x, g_gui_info.nav_window_pos_y}, ImGuiCond_Always);
		ImGui::SetNextWindowSize({g_gui_info.nav_window_width, 0.f}, ImGuiCond_Always);

		if (ImGui::Begin("navigation", 0, window_flags))
		{
			for (std::pair<tabs, navigation_struct> nav_item : g_gui_service->get_navigation())
			{
				switch (nav_item.first)
				{
				case tabs::PLAYER: continue;
				default: components::nav_item(nav_item, 0);
				}
			}
			ImGui::End();
		}
	}
}
