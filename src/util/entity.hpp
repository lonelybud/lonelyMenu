#pragma once
#include "pointers.hpp"

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

		auto hnd = g_pointers->m_gta.m_handle_to_ptr(ent);

		if (!hnd || !hnd->m_net_object || !*g_pointers->m_gta.m_is_session_started)
			return false;

		if (network_has_control_of_entity(hnd->m_net_object))
			return true;

		return false;
	}
}
