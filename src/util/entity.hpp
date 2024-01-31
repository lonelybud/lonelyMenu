#pragma once
#include "core/data/misc.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "scripts.hpp"
#include "services/notifications/notification_service.hpp"

namespace big::entity
{
	std::vector<Entity> get_entities(bool vehicles, bool peds, bool props = false, bool include_self_veh = false);
	bool load_ground_at_3dcoord(Vector3& location);

	inline bool network_has_control_of_entity(rage::netObject* net_object)
	{
		return !net_object || !net_object->m_next_owner_id && (net_object->m_control_id == -1);
	}

	inline bool take_control_of(rage::CDynamicEntity* hnd)
	{
		if (!*g_pointers->m_gta.m_is_session_started)
			return true;

		auto n_of_try = g_misc.request_control ? 300 : 1;

		for (int i = 0; i < n_of_try; ++i)
		{
			if (hnd && hnd->m_net_object && network_has_control_of_entity(hnd->m_net_object))
				return true;

			if (n_of_try > 1 && hnd && hnd->m_net_object)
			{
				g_pointers->m_gta.m_request_control(hnd->m_net_object);
				script::get_current()->yield();
			}
		}

		return false;
	}

	inline bool take_control_of(Entity ent)
	{
		return take_control_of(g_pointers->m_gta.m_handle_to_ptr(ent));
	}

	inline bool request_model(Hash hash)
	{
		int attempt = 0;

		if (STREAMING::IS_MODEL_VALID(hash) && STREAMING::IS_MODEL_IN_CDIMAGE(hash))
			while (!STREAMING::HAS_MODEL_LOADED(hash) && attempt < 100)
			{
				STREAMING::REQUEST_MODEL(hash);
				++attempt;
				script::get_current()->yield();
			}

		if (!STREAMING::HAS_MODEL_LOADED(hash))
		{
			g_notification_service->push_warning("Failed", "Failed to load modal");
			return false;
		}

		return true;
	}

	double distance_to_middle_of_screen(const rage::fvector2& screen_pos);
	Entity get_entity_closest_to_middle_of_screen(rage::fwEntity** pointer = nullptr, std::vector<Entity> ignore_entities = {}, bool include_veh = true, bool include_ped = true, bool include_prop = true, bool include_players = true);
}
