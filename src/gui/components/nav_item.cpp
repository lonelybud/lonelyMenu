#include "components.hpp"
#include "core/data/gui_info.hpp"
#include "core/settings/window.hpp"
#include "services/gui/gui_service.hpp"

namespace big
{
	void components::nav_item(std::pair<tabs, navigation_struct>& navItem, int nested)
	{
		const bool current_tab = !g_gui_service->get_selected_tab().empty() && g_gui_service->get_selected_tab().size() > nested
		    && navItem.first == g_gui_service->get_selected_tab().at(nested);

		if (current_tab)
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.29f, 0.45f, 0.69f, 1.f));

		const char* key = navItem.second.name;
		if (components::nav_button(key))
		{
			g_gui_service->set_selected(navItem.first);
			g_gui_info.update_gui_info();
		}

		if (current_tab)
			ImGui::PopStyleColor();

		if (current_tab && !navItem.second.sub_nav.empty())
		{
			ImDrawList* draw_list = ImGui::GetForegroundDrawList();

			for (std::pair<tabs, navigation_struct> item : navItem.second.sub_nav)
			{
				// window top + relative cursor distance
				auto y = g_gui_info.nav_window_pos_y + ImGui::GetCursorPosY();

				draw_list->AddRectFilled({g_gui_info.nav_window_pos_x, y},
				    {g_gui_info.nav_window_pos_x + g_gui_info.nav_window_width, y + g_gui_info.nav_btn_height},
				    ImGui::ColorConvertFloat4ToU32({1.f, 1.f, 1.f, .15f + (.075f * nested)}));
				nav_item(item, nested + 1);
			}
		}
	}
}
