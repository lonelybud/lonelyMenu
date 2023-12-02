#include "entity.hpp"

#include "gta_util.hpp"
#include "pools.hpp"

namespace big::entity
{
	std::vector<Entity> get_entities(bool vehicles, bool peds, bool props, bool include_self_veh)
	{
		std::vector<Entity> target_entities;

		if (vehicles)
		{
			for (auto vehicle : pools::get_all_vehicles())
			{
				if (!include_self_veh && vehicle == gta_util::get_local_vehicle())
					continue;

				target_entities.push_back(g_pointers->m_gta.m_ptr_to_handle(vehicle));
			}
		}

		if (peds)
		{
			for (auto ped : pools::get_all_peds())
			{
				if (ped == g_local_player)
					continue;

				target_entities.push_back(g_pointers->m_gta.m_ptr_to_handle(ped));
			}
		}

		if (props)
		{
			for (auto prop : pools::get_all_props())
			{
				target_entities.push_back(g_pointers->m_gta.m_ptr_to_handle(prop));
			}
		}

		return target_entities;
	}

	bool load_ground_at_3dcoord(Vector3& location)
	{
		float groundZ;
		bool done = false;

		for (int i = 0; i < 10; i++)
		{
			for (int z = 0; z < 1000; z += 25)
			{
				float ground_iteration = static_cast<float>(z);
				// Only request a collision after the first try failed because the location might already be loaded on first attempt.
				if (i >= 1 && (z % 100 == 0))
				{
					STREAMING::REQUEST_COLLISION_AT_COORD(location.x, location.y, ground_iteration);
					script::get_current()->yield();
				}

				if (MISC::GET_GROUND_Z_FOR_3D_COORD(location.x, location.y, ground_iteration, &groundZ, false, false))
				{
					location.z = groundZ + 1.f;
					done       = true;
					break;
				}
			}

			float height;
			if (done && WATER::GET_WATER_HEIGHT(location.x, location.y, location.z, &height))
			{
				location.z = height + 1.f;
			}

			if (done)
			{
				return true;
			}
		}

		location.z = 1000.f;

		return false;
	}
}