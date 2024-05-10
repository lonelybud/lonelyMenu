#include "entity.hpp"

#include "gta_util.hpp"
#include "natives.hpp"
#include "pools.hpp"
#include "services/players/player_service.hpp"

namespace big::entity
{
	std::vector<Entity> get_entities(bool vehicles, bool peds, bool props)
	{
		std::vector<Entity> target_entities;

		if (vehicles)
			for (auto vehicle : pools::get_all_vehicles())
				if (vehicle)
					target_entities.push_back(g_pointers->m_gta.m_ptr_to_handle(vehicle));


		if (peds)
			for (auto ped : pools::get_all_peds())
			{
				if (!ped || ped == g_local_player)
					continue;

				target_entities.push_back(g_pointers->m_gta.m_ptr_to_handle(ped));
			}

		if (props)
			for (auto prop : pools::get_all_props())
				if (prop)
					target_entities.push_back(g_pointers->m_gta.m_ptr_to_handle(prop));

		return target_entities;
	}

	bool load_ground_at_3dcoord(Vector3& location)
	{
		constexpr float max_ground_check = 1000.f;
		float ground_z                   = location.z;
		int current_attempts             = 0;
		bool found_ground;

		do
		{
			found_ground = MISC::GET_GROUND_Z_FOR_3D_COORD(location.x, location.y, max_ground_check, &ground_z, FALSE, FALSE);
			STREAMING::REQUEST_COLLISION_AT_COORD(location.x, location.y, location.z);

			if (current_attempts % 10 == 0)
				location.z += 25.f;

			++current_attempts;

			script::get_current()->yield();
		} while (!found_ground && location.z <= max_ground_check);

		if (!found_ground)
			return false;

		float height;
		if (WATER::GET_WATER_HEIGHT(location.x, location.y, location.z, &height))
			location.z = height;
		else
			location.z = ground_z;

		return true;
	}

	double distance_to_middle_of_screen(const rage::fvector2& screen_pos)
	{
		double cumulative_distance{};

		if (screen_pos.x > 0.5)
			cumulative_distance += screen_pos.x - 0.5;
		else
			cumulative_distance += 0.5 - screen_pos.x;

		if (screen_pos.y > 0.5)
			cumulative_distance += screen_pos.y - 0.5;
		else
			cumulative_distance += 0.5 - screen_pos.y;

		return cumulative_distance;
	}

	Entity get_entity_closest_to_middle_of_screen(rage::fwEntity** pointer, std::vector<Entity> ignore_entities, bool include_veh, bool include_ped, bool include_prop, bool include_players)
	{
		Entity closest_entity{};
		rage::fwEntity* closest_entity_ptr = nullptr;
		float distance                     = 1;

		auto ignored_entity = [=](Entity handle) -> bool {
			if (handle == self::ped)
				return true;

			for (auto ent : ignore_entities)
			{
				if (handle == ent)
					return true;
			}

			return false;
		};

		auto update_closest_entity = [&](Entity handle, rage::fwEntity* entity_ptr) {
			Vector3 pos = ENTITY::GET_ENTITY_COORDS(handle, 0);
			rage::fvector2 screenpos;
			HUD::GET_HUD_SCREEN_POSITION_FROM_WORLD_POSITION(pos.x, pos.y, pos.z, &screenpos.x, &screenpos.y);

			if (distance_to_middle_of_screen(screenpos) < distance && ENTITY::HAS_ENTITY_CLEAR_LOS_TO_ENTITY(self::ped, handle, 17) && !ignored_entity(handle))
			{
				closest_entity     = handle;
				closest_entity_ptr = entity_ptr;
				distance           = distance_to_middle_of_screen(screenpos);
			}
		};

		auto include_pool = [&](auto& pool) {
			for (const auto ptr : pool())
				if (ptr)
					update_closest_entity(g_pointers->m_gta.m_ptr_to_handle(ptr), ptr);
		};

		if (include_veh)
			include_pool(pools::get_all_vehicles);

		if (include_ped)
			include_pool(pools::get_all_peds);

		if (include_prop)
			include_pool(pools::get_all_props);

		if (include_players)
		{
			for (auto player : g_player_service->players() | std::ranges::views::values)
			{
				if (player->get_ped())
				{
					Ped handle = g_pointers->m_gta.m_ptr_to_handle(player->get_ped());
					update_closest_entity(handle, player->get_ped());
				}
			}
		}

		if (closest_entity_ptr)
			*pointer = closest_entity_ptr;

		return closest_entity;
	}
}