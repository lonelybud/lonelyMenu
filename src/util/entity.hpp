#pragma once
#include "core/data/misc.hpp"
#include "pointers.hpp"
#include "scripts.hpp"

namespace big::entity
{
	std::vector<Entity> get_entities(bool vehicles, bool peds, bool props = false, bool include_self_veh = false);
	bool load_ground_at_3dcoord(Vector3& location);

	inline bool network_has_control_of_entity(rage::netObject* net_object)
	{
		return !net_object || !net_object->m_next_owner_id && (net_object->m_control_id == -1);
	}

	inline bool take_control_of(Entity ent)
	{
		if (!*g_pointers->m_gta.m_is_session_started)
			return true;

		auto n_of_try = g_misc.request_control ? 300 : 1;

		for (int i = 0; i < n_of_try; ++i)
		{
			auto hnd = g_pointers->m_gta.m_handle_to_ptr(ent);

			if (hnd && hnd->m_net_object && network_has_control_of_entity(hnd->m_net_object))
				return true;

			if (n_of_try > 1)
			{
				g_pointers->m_gta.m_request_control(hnd->m_net_object);
				script::get_current()->yield();
			}
		}

		return false;
	}

	double distance_to_middle_of_screen(const rage::fvector2& screen_pos);
	Entity get_entity_closest_to_middle_of_screen(rage::fwEntity** pointer = nullptr, std::vector<Entity> ignore_entities = {}, bool include_veh = true, bool include_ped = true, bool include_prop = true, bool include_players = true);
}
