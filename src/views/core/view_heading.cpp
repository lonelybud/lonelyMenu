#include "core/data/gui_info.hpp"
#include "core/settings/window.hpp"
#include "gta_util.hpp"
#include "views/view.hpp"

namespace big
{
	void view::heading()
	{
		ImGui::SetNextWindowSize({g_gui_info.nav_window_width, 80.f * g_window.gui_scale});
		ImGui::SetNextWindowPos({g_gui_info.nav_window_pos_x, g_gui_info.nav_window_pos_x});
		if (ImGui::Begin("menu_heading", nullptr, window_flags | ImGuiWindowFlags_NoScrollbar))
		{
			ImGui::Text("Welcome");
			// only show when offline
			if (auto network = gta_util::get_network(); network && network->m_game_session_state == 0)
			{
				ImGui::SameLine();
				ImGui::SetCursorPos(
				    {g_gui_info.nav_window_width - ImGui::CalcTextSize("Unload").x - (window_padding.x * g_window.gui_scale),
				        window_padding.y * g_window.gui_scale});
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.69f, 0.29f, 0.29f, 1.00f));
				if (components::nav_button("Unload"))
					g_running = false;
				ImGui::PopStyleColor();
			}
		}
		ImGui::End();
	}
}
