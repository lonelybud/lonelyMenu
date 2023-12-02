#include "view_esp.hpp"

#include "core/scr_globals.hpp"
#include "core/settings/esp.hpp"
#include "gta/enums.hpp"
#include "util/math.hpp"

namespace big
{
	void esp::draw_player(const player_ptr& plyr, ImDrawList* const draw_list)
	{
		if (!plyr->is_valid() || !plyr->get_ped() || !plyr->get_ped()->m_navigation)
			return;

		auto& player_pos = *plyr->get_ped()->m_navigation->get_position();

		float screen_x, screen_y;

		const float distance = math::calculate_distance_from_game_cam(player_pos);
		const float multplr  = distance > g_esp.global_render_distance[1] ? -1.f : 6.17757f / distance;

		if (multplr == -1.f || g_esp.global_render_distance[0] > distance)
			return;

		uint32_t ped_damage_bits = plyr->get_ped()->m_damage_bits;

		if (g_pointers->m_gta.m_get_screen_coords_for_world_coords(player_pos.data, &screen_x, &screen_y))
		{
			const auto esp_x = (float)*g_pointers->m_gta.m_resolution_x * screen_x;
			const auto esp_y = (float)*g_pointers->m_gta.m_resolution_y * screen_y;

			std::string name_str = plyr->get_name();
			ImVec2 name_pos      = {esp_x - (62.5f * multplr), esp_y - (175.f * multplr) - 20.f};

			if (g_esp.distance)
				name_str += " | " + std::to_string((int)distance) + "m";

			draw_list->AddText(name_pos, g_esp.default_color, name_str.c_str());
		}
	}

	void esp::draw_object(const rage::CDynamicEntity* object, ImDrawList* const draw_list, std::string name)
	{
		if (!object || !object->m_navigation)
			return;

		auto& object_pos = *object->m_navigation->get_position();

		float screen_x, screen_y;

		const float distance = math::calculate_distance_from_game_cam(object_pos);
		const float multplr  = distance > g_esp.global_render_distance[1] ? -1.f : 6.17757f / distance;

		if (multplr == -1.f || g_esp.global_render_distance[0] > distance)
			return;

		ImVec2 name_pos;

		if (g_pointers->m_gta.m_get_screen_coords_for_world_coords(object_pos.data, &screen_x, &screen_y))
		{
			const auto esp_x = (float)*g_pointers->m_gta.m_resolution_x * screen_x;
			const auto esp_y = (float)*g_pointers->m_gta.m_resolution_y * screen_y;
			name_pos         = {esp_x, esp_y};
		}
		else
		{
			return;
		}

		draw_list->AddText(name_pos, g_esp.default_color, name.c_str());
	}


	void esp::draw()
	{
		if (!g_esp.enabled)
			return;

		static int tick_count = 31;

		if (tick_count > g_esp.tick_count_threshhold)
		{
			tick_count = 1;

			if (const auto draw_list = ImGui::GetBackgroundDrawList(); draw_list)
			{
				if (g_esp.show_player)
					g_player_service->iterate([draw_list](const player_entry& entry) {
						draw_player(entry.second, draw_list);
					});

				if (g_esp.show_gs_cache_boxes && *g_pointers->m_gta.m_script_globals && **g_pointers->m_gta.m_script_globals)
					if (auto gs_cache_box_entity = *scr_globals::pickups.at(605).as<Entity*>(); gs_cache_box_entity != 0)
						draw_object(g_pointers->m_gta.m_handle_to_ptr(gs_cache_box_entity), draw_list, "G's Cache");
			}
		}
		else
			++tick_count;
	}
}