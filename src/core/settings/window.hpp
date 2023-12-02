#pragma once
#include <imgui.h>

namespace big
{
	inline struct g_window_t
	{
		ImU32 background_color = 3556769792;
		ImU32 text_color       = 4292072403;
		ImU32 button_color     = 2936035029;
		ImU32 frame_color      = 2939170865;
		float gui_scale        = 0.75;

		ImFont* font_title     = nullptr;
		ImFont* font_sub_title = nullptr;
		ImFont* font_small     = nullptr;
		ImFont* font_icon      = nullptr;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(g_window_t, background_color, text_color, button_color, frame_color, gui_scale)
	} g_window{};
}