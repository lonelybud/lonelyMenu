#include "core/settings/menu.hpp"
#include "core/settings/window.hpp"
#include "gui.hpp"
#include "renderer/renderer.hpp"
#include "views/view.hpp"
#include "widgets/imgui_hotkey.hpp"

namespace big
{
	void view::gui_settings()
	{
		components::sub_title("Menu Settings");
		ImGui::Hotkey("Toggle Key", &g_menu.menu_toggle);

		components::sub_title("UI Scale");
		static float scale_value = g_window.gui_scale;
		ImGui::SliderFloat("##gui-scale", &scale_value, 0.75f, 1.5f, "%.2f");
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			g_window.gui_scale = scale_value;
			g_renderer.rescale(g_window.gui_scale);
		}

		components::sub_title("UI Colors");
		static ImVec4 col_gui = ImGui::ColorConvertU32ToFloat4(g_window.background_color);
		if (ImGui::ColorEdit4("Gui Color", (float*)&col_gui, ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoSidePreview))
			g_window.background_color = ImGui::ColorConvertFloat4ToU32(col_gui);

		static ImVec4 col_text = ImGui::ColorConvertU32ToFloat4(g_window.text_color);
		if (ImGui::ColorEdit4("Text Color", (float*)&col_text, ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoSidePreview))
			g_window.text_color = ImGui::ColorConvertFloat4ToU32(col_text);

		static ImVec4 col_button = ImGui::ColorConvertU32ToFloat4(g_window.button_color);
		if (ImGui::ColorEdit4("Button Color", (float*)&col_button, ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoSidePreview))
			g_window.button_color = ImGui::ColorConvertFloat4ToU32(col_button);

		static ImVec4 col_frame = ImGui::ColorConvertU32ToFloat4(g_window.frame_color);
		if (ImGui::ColorEdit4("Frame Color", (float*)&col_frame, ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoSidePreview))
			g_window.frame_color = ImGui::ColorConvertFloat4ToU32(col_frame);
	}
}
