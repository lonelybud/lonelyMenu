#include "core/data/infractions.hpp"
#include "fiber_pool.hpp"
#include "fonts/fonts.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "services/gui/gui_service.hpp"
#include "services/players/player_service.hpp"
#include "util/strings.hpp"
#include "views/view.hpp"

#define IMGUI_DEFINE_PLACEMENT_NEW
#define IMGUI_DEFINE_MATH_OPERATORS
#include "core/settings/window.hpp"

#include <imgui_internal.h>

namespace big
{
	static bool has_scrollbar = false;

	static void player_button(const player_ptr& plyr)
	{
		if (plyr == nullptr)
			return;

		bool selected_player = plyr == g_player_service->get_selected();

		// generate icons string
		std::string player_icons;
		if (plyr->is_host())
			player_icons += FONT_ICON_HOST;
		if (plyr->is_friend())
			player_icons += FONT_ICON_FRIEND;
		if (const auto ped = plyr->get_ped(); (ped != nullptr && ped->m_ped_task_flag & (uint8_t)ePedTask::TASK_DRIVING))
			player_icons += FONT_ICON_VEHICLE;

		const auto player_iconsc    = player_icons.c_str();
		const auto player_icons_end = player_iconsc + player_icons.size();

		// calculate icons width
		const auto window = ImGui::GetCurrentWindow();
		ImGui::PushFont(g_window.font_icon);
		const auto icons_size = ImGui::CalcTextSize(player_iconsc, player_icons_end);
		const ImVec2 icons_pos(window->DC.CursorPos.x + 300.0f - 32.0f - icons_size.x, window->DC.CursorPos.y + 2.0f);
		const ImRect icons_box(icons_pos, icons_pos + icons_size);
		ImGui::PopFont();

		if (plyr->is_modder)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.1f, 0.1f, 1.f)); // red
		else if (plyr->is_spammer)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.67f, 0.f, 1.f)); // more of yellow than orange
		else if (plyr->is_blocked)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.73f, 0.f, 1.f, 1.f)); // purple
		else if (plyr->is_known_player)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.25f, 0.25f, 1.f)); // grey
		else if (plyr->is_toxic)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.45f, 0.f, 1.f)); // green dark

		if (selected_player)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.29f, 0.45f, 0.69f, 1.f)); // blue

		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, {0.0, 0.5});
		ImGui::PushID(plyr->id());

		const auto style = ImGui::GetStyle();
		// branchless conditional calculation
		const auto plyr_btn_width = 300.f - (style.ItemInnerSpacing.x * 2) - (has_scrollbar * style.ScrollbarSize);
		if (ImGui::Button(plyr->id() == self::id ? "you" : plyr->get_name(), {plyr_btn_width, 0.f}))
		{
			g_player_service->set_selected(plyr);
			g_gui_service->set_selected(tabs::PLAYER);
		}
		if (ImGui::IsItemHovered() && !plyr->infractions.empty())
		{
			ImGui::BeginTooltip();
			for (auto infraction : plyr->infractions)
				ImGui::BulletText(infraction_desc[(Infraction)infraction]);
			ImGui::EndTooltip();
		}

		ImGui::PopID();
		ImGui::PopStyleVar();

		if (selected_player)
			ImGui::PopStyleColor();

		if (plyr->is_blocked || plyr->is_spammer || plyr->is_modder || plyr->is_toxic || plyr->is_known_player)
			ImGui::PopStyleColor();

		// render icons on top of the player button
		ImGui::PushFont(g_window.font_icon);
		ImGui::RenderTextWrapped(icons_box.Min, player_iconsc, player_icons_end, icons_size.x);
		ImGui::PopFont();
	}

	void view::players()
	{
		// player count does not include ourself that's why +1
		const auto player_count = g_player_service->players().size() + 1;

		if (!*g_pointers->m_gta.m_is_session_started && player_count < 2)
			return;
		float window_pos = 110.f + g_gui_service->nav_ctr * ImGui::CalcTextSize("W").y
		    + g_gui_service->nav_ctr * ImGui::GetStyle().ItemSpacing.y
		    + g_gui_service->nav_ctr * ImGui::GetStyle().ItemInnerSpacing.y + ImGui::GetStyle().WindowPadding.y;

		ImGui::SetNextWindowSize({300.f, 0.f});
		ImGui::SetNextWindowPos({10.f, window_pos});
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {2.0f, 2.0f});

		if (ImGui::Begin("playerlist", nullptr, window_flags))
		{
			const auto style = ImGui::GetStyle();
			float window_height = (ImGui::CalcTextSize("A").y + style.FramePadding.y * 2.0f + style.ItemSpacing.y) // button size
			        * player_count                                       // amount of players
			    + (player_count > 1) * ((style.ItemSpacing.y * 2) + 1.f) // account for ImGui::Separator spacing
			    + (player_count == 1) * 2.f;                             // some arbitrary height to make it fit
			// used to account for scrollbar width
			has_scrollbar = window_height + window_pos > (float)*g_pointers->m_gta.m_resolution_y - 10.f;

			// basically whichever is smaller, the max available screenspace or the calculated window_height
			window_height = has_scrollbar ? (float)*g_pointers->m_gta.m_resolution_y - (window_pos + 40.f + 25) : window_height;

			ImGui::PushStyleColor(ImGuiCol_FrameBg, {0.f, 0.f, 0.f, 0.f});
			ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, {0.f, 0.f, 0.f, 0.f});

			auto width_of_list = ImGui::GetWindowSize().x - ImGui::GetStyle().WindowPadding.x * 2;

			static std::string search_player_name;
			ImGui::SetNextItemWidth(width_of_list);
			components::input_text_with_hint("###search_player_name", "search name", search_player_name);

			if (ImGui::BeginListBox("##players", {width_of_list, window_height}))
			{
				ImGui::SetScrollX(0);
				player_button(g_player_service->get_self());

				if (player_count > 1)
				{
					ImGui::Separator();

					for (const auto& [_, player] : g_player_service->players())
						if (search_player_name.length())
						{
							std::string lowercaseSearchString = toLowercase(search_player_name);
							if (std::string lowercaseStr = toLowercase(player->get_name()); lowercaseStr.find(lowercaseSearchString) != std::string::npos)
								player_button(player);
						}
						else
							player_button(player);
				}

				ImGui::EndListBox();
			}
			ImGui::PopStyleColor(2);
		}

		ImGui::PopStyleVar();
		ImGui::End();
	}
}
