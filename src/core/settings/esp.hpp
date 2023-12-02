#pragma once
#include <imgui.h>

namespace big
{
	struct g_esp_t
	{
		bool enabled                    = false;
		float global_render_distance[2] = {0.f, 600.f};
		bool show_player                = true;
		bool distance                   = false;
		bool show_gs_cache_boxes        = false;

		int tick_count_threshhold = 30;

		ImU32 default_color = 4285713522;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(g_esp_t, global_render_distance, default_color, tick_count_threshhold)
	};
	inline g_esp_t g_esp{};
}