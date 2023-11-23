#include "entity.hpp"

namespace big::entity
{
	bool network_has_control_of_entity(rage::netObject* net_object)
	{
		return !net_object || !net_object->m_next_owner_id && (net_object->m_control_id == -1);
	}

	bool take_control_of(Entity ent, int n_of_tries)
	{
		if (!*g_pointers->m_gta.m_is_session_started)
			return true;

		for (int i = 0; i < n_of_tries; ++i)
		{
			if (auto hnd = g_pointers->m_gta.m_handle_to_ptr(ent); hnd && hnd->m_net_object)
			{
				g_pointers->m_gta.m_request_control(hnd->m_net_object);

				if (hnd && network_has_control_of_entity(hnd->m_net_object))
					return true;
			}

			script::get_current()->yield(10ms);
		}

		return false;
	}
}