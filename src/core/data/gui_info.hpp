#pragma once
#include "../settings/window.hpp"
#include "gui.hpp"
#include "pointers.hpp"
#include "services/gui/gui_service.hpp"
#include "services/players/player_service.hpp"

#include <imgui.h>

namespace big
{
	constexpr auto window_padding  = ImVec2(15, 15);
	constexpr auto frame_padding   = ImVec2(5, 5);
	constexpr auto item_spacing    = ImVec2(8, 6);
	constexpr auto scroll_bar_size = 15.0f;

	class gui_info
	{
	public:
		float button_height;

		float nav_window_pos_y;
		float nav_window_pos_x;
		float nav_window_width;
		float nav_window_pos_y2;

		float nav_btn_padding_y;
		float nav_btn_spacing_y;
		float nav_btn_height;

		float plrs_wind_margin;
		float plrs_wind_width;
		float plrs_wind_padding;
		float plrs_wind_content_width;
		float plrs_wind_pos_y;
		float plr_btn_width;
		float plrs_list_height;

		inline void update_gui_info()
		{
			if (!g_gui->is_open())
				return;

			auto font_size = ImGui::GetFontSize();
			auto nav_count = g_gui_service->get_rendered_navs_count();

			button_height = font_size + (frame_padding.y * 2 + item_spacing.y) * g_window.gui_scale;

			nav_btn_padding_y = 5 * g_window.gui_scale;
			nav_btn_spacing_y = 2 * g_window.gui_scale;
			nav_btn_height    = font_size + nav_btn_padding_y * 2 + nav_btn_spacing_y;

			nav_window_pos_y = 100.f * g_window.gui_scale;
			nav_window_pos_x = 10.f * g_window.gui_scale;
			nav_window_width = 300.f * g_window.gui_scale;
			// window y + no of navs - last button spacing + vertical padding of window
			nav_window_pos_y2 =
			    nav_window_pos_y + nav_count * nav_btn_height - nav_btn_spacing_y + (window_padding.y * g_window.gui_scale * 2);

			// players count + 1 (include yourself)
			const auto player_count      = g_player_service->players().size() + 1;
			plrs_wind_margin             = 10.f * g_window.gui_scale;
			plrs_wind_padding            = 2.0f * g_window.gui_scale;
			auto plrs_wind_padding_total = 2 * plrs_wind_padding;
			plrs_wind_content_width      = nav_window_width - plrs_wind_padding_total;
			plrs_wind_pos_y              = nav_window_pos_y2 + plrs_wind_margin;
			//
			auto plrs_wind_raw_height = button_height * player_count + 5.f; // i dont know the reason but i have to add 5.f or smaller to let the scrollbar removed when one only player
			// screen Y - (window Y pos + searchbar height + window padding + window bottom margin)
			auto plrs_wind_max_height = (float)*g_pointers->m_gta.m_resolution_y - (plrs_wind_pos_y + button_height + plrs_wind_padding_total + plrs_wind_margin);
			auto plrs_wind_has_scrollbar = plrs_wind_raw_height > plrs_wind_max_height;
			plrs_list_height             = plrs_wind_has_scrollbar ? plrs_wind_max_height : plrs_wind_raw_height;

			plr_btn_width = nav_window_width - plrs_wind_padding_total - (plrs_wind_has_scrollbar * scroll_bar_size);
		}
	};

	inline gui_info g_gui_info;
}
