#include "core/data/gui_info.hpp"
#include "core/settings/window.hpp"
#include "core/vars.hpp"
#include "fiber_pool.hpp"
#include "fonts/fonts.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "services/gui/gui_service.hpp"
#include "services/players/player_service.hpp"
#include "views/view.hpp"

#define IMGUI_DEFINE_PLACEMENT_NEW
#define IMGUI_DEFINE_MATH_OPERATORS
#include "core/settings/window.hpp"

#include <imgui_internal.h>

namespace big
{
	static void player_button(const player_ptr& plyr)
	{
		if (plyr == nullptr)
			return;

		bool selected_player = plyr == g_player_service->get_selected();
		bool is_friend       = plyr->is_friend();

		// generate icons string
		std::string player_icons;
		if (plyr->is_host())
			player_icons += FONT_ICON_HOST;
		if (is_friend)
			player_icons += FONT_ICON_FRIEND;
		if (const auto ped = plyr->get_ped(); (ped != nullptr && ped->m_ped_task_flag & (uint8_t)ePedTask::TASK_DRIVING))
			player_icons += FONT_ICON_VEHICLE;

		const auto player_iconsc    = player_icons.c_str();
		const auto player_icons_end = player_iconsc + player_icons.size();

		// calculate icons width
		const auto window = ImGui::GetCurrentWindow();
		ImGui::PushFont(g_window.font_icon);
		const auto icons_size = ImGui::CalcTextSize(player_iconsc, player_icons_end);
		const ImVec2 icons_pos(window->DC.CursorPos.x + g_gui_info.plr_btn_width - icons_size.x - frame_padding.x,
		    window->DC.CursorPos.y + 2.0f);
		const ImRect icons_box(icons_pos, icons_pos + icons_size);
		ImGui::PopFont();

		bool special_color_applied = true;

		if (plyr->is_pain_in_ass)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.f)); // red
		else if (plyr->is_spammer)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.67f, 0.f, 1.f)); // more of yellow than orange
		else if (plyr->plyr_type == player_type::modder)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.2f, 0.2f, 1.f)); // light red
		else if (plyr->is_blocked)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.73f, 0.f, 1.f, 1.f)); // purple
		else if (plyr->plyr_type == player_type::other)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.45f, 0.f, 1.f)); // green dark
		else if (is_friend)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.2f, 0.52f, 1.f)); // pink
		else if (plyr->is_known_player)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.25f, 0.25f, 1.f)); // grey
		else
			special_color_applied = false;

		if (selected_player)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.29f, 0.45f, 0.69f, 1.f)); // blue

		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, {0.0, 0.5});
		ImGui::PushID(plyr->id());

		if (ImGui::Button(plyr->id() == self::id ? "you" : plyr->m_name, {g_gui_info.plr_btn_width, 0.f}))
		{
			g_player_service->set_selected(plyr);
			g_gui_service->set_selected(tabs::PLAYER);
			g_gui_info.update_gui_info();
		}

		ImGui::PopID();
		ImGui::PopStyleVar();

		if (selected_player)
			ImGui::PopStyleColor();

		if (special_color_applied)
			ImGui::PopStyleColor();

		// render icons on top of the player button
		ImGui::PushFont(g_window.font_icon);
		ImGui::RenderTextWrapped(icons_box.Min, player_iconsc, player_icons_end, icons_size.x);
		ImGui::PopFont();
	}

	void view::players()
	{
		ImGui::SetNextWindowSize({g_gui_info.nav_window_width, 0.f});
		ImGui::SetNextWindowPos({g_gui_info.nav_window_pos_x, g_gui_info.plrs_wind_pos_y});
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {g_gui_info.plrs_wind_padding, g_gui_info.plrs_wind_padding});
		if (ImGui::Begin("playerlist", nullptr, window_flags))
		{
			ImGui::PushStyleColor(ImGuiCol_FrameBg, {0.f, 0.f, 0.f, 0.f});
			ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, {0.f, 0.f, 0.f, 0.f});

			// render search
			static std::string search_player_name;
			ImGui::SetNextItemWidth(g_gui_info.plrs_wind_content_width);
			if (components::input_text_with_hint("###search_player_name", "search name", search_player_name))
				std::transform(search_player_name.begin(), search_player_name.end(), search_player_name.begin(), ::tolower);
			//

			if (ImGui::BeginListBox("##players", {g_gui_info.plrs_wind_content_width, g_gui_info.plrs_list_height}))
			{
				player_button(g_player_service->get_self());

				for (const auto& [_, player] : g_player_service->players())
					if (search_player_name.length())
					{
						std::string lower_case_name = player->m_name;
						std::transform(lower_case_name.begin(), lower_case_name.end(), lower_case_name.begin(), ::tolower);

						if (lower_case_name.find(search_player_name) != std::string::npos)
							player_button(player);
					}
					else
						player_button(player);

				ImGui::EndListBox();
			}
			ImGui::PopStyleColor(2);
		}

		ImGui::PopStyleVar();
		ImGui::End();
	}
}
