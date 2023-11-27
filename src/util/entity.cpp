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
}