#include "backend/looped_command.hpp"
#include "core/data/gui_info.hpp"
#include "core/settings/window.hpp"
#include "fiber_pool.hpp"
#include "script_mgr.hpp"
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
			ImGui::SameLine();
			ImGui::SetCursorPos(
			    {g_gui_info.nav_window_width - ImGui::CalcTextSize("Unload").x - (window_padding.x * g_window.gui_scale),
			        window_padding.y * g_window.gui_scale});
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.69f, 0.29f, 0.29f, 1.00f));
			if (components::nav_button("Unload"))
			{
				// allow to unload in the main title screen.
				if (g_script_mgr.can_tick())
				{
					// empty the pool, we want the that job below run no matter what for clean up purposes.
					g_fiber_pool->reset();
					g_fiber_pool->queue_job([] {
						for (auto& command : g_looped_commands)
							if (command->is_enabled())
								command->on_disable();

						g_running = false;
					});
				}
				else
				{
					g_running = false;
				}
			}
			ImGui::PopStyleColor();
		}
		ImGui::End();
	}
}
