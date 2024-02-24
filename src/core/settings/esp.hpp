#pragma once
#include <imgui.h>

namespace big
{
	struct g_esp_t
	{
		bool enabled                    = false;
		bool distance                   = false;
		float global_render_distance[2] = {0.f, 600.f};
		ImU32 default_color             = 4285713522;
		ImU32 enemy_color               = 4281479904;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(g_esp_t, global_render_distance, default_color, enemy_color)
	};
	inline g_esp_t g_esp{};
}