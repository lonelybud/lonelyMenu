#include "core/data/gui_info.hpp"
#include "gui/components/components.hpp"
#include "pointers.hpp"

namespace big
{
	bool components::nav_button(const std::string_view text)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, {0.f, 0.5f});
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {0, g_gui_info.nav_btn_padding_y});
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0, g_gui_info.nav_btn_spacing_y});
		bool result = ImGui::Button(text.data(), {-1, 0});
		ImGui::PopStyleVar(3);
		ImGui::PopStyleColor(3);

		return result;
	}
}