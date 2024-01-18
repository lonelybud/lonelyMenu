#include "backend/looped_command.hpp"
#include "core/settings/window.hpp"
#include "fiber_pool.hpp"
#include "script_mgr.hpp"
#include "views/view.hpp"

namespace big
{
	void view::heading()
	{
		ImGui::SetNextWindowSize({300.f * g_window.gui_scale, 80.f * g_window.gui_scale});
		ImGui::SetNextWindowPos({10.f, 10.f});
		if (ImGui::Begin("menu_heading", nullptr, window_flags | ImGuiWindowFlags_NoScrollbar))
		{
			ImGui::Text("Welcome");
			ImGui::SameLine();
			ImGui::SetCursorPos(
			    {(300.f * g_window.gui_scale) - ImGui::CalcTextSize("Unload").x - ImGui::GetStyle().ItemSpacing.x,
			        ImGui::GetStyle().WindowPadding.y / 2 + ImGui::GetStyle().ItemSpacing.y + (ImGui::CalcTextSize("W").y / 2)});
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
