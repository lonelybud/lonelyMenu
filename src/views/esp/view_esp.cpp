#include "view_esp.hpp"

#include "core/settings/esp.hpp"
#include "services/players/player_service.hpp"
#include "util/math.hpp"

namespace big::esp
{
	static inline void draw_player(const player_ptr& plyr, ImDrawList* const draw_list)
	{
		if (!plyr->is_valid() || !plyr->get_ped() || !plyr->get_ped()->m_navigation)
			return;

		auto& player_pos     = *plyr->get_ped()->m_navigation->get_position();
		const float distance = math::calculate_distance_from_game_cam(player_pos);
		if (distance > g_esp.global_render_distance[1] || g_esp.global_render_distance[0] > distance)
			return;

		float screen_x, screen_y;
		const float multplr = 6.17757f / distance;

		if (g_pointers->m_gta.m_get_screen_coords_for_world_coords(player_pos.data, &screen_x, &screen_y))
		{
			const auto esp_x = (float)*g_pointers->m_gta.m_resolution_x * screen_x;
			const auto esp_y = (float)*g_pointers->m_gta.m_resolution_y * screen_y;

			std::string name_str = plyr->get_name();
			ImVec2 name_pos      = {esp_x - (62.5f * multplr), esp_y - (175.f * multplr) - 20.f};

			if (g_esp.distance)
				name_str += " | " + std::to_string((int)distance) + "m";

			draw_list->AddText(name_pos, plyr->esp_enemy ? g_esp.enemy_color : g_esp.default_color, name_str.c_str());
		}
	}

	void draw()
	{
		if (g_esp.enabled)
			for (auto& entry : g_player_service->players())
				draw_player(entry.second, ImGui::GetBackgroundDrawList());
	}
}