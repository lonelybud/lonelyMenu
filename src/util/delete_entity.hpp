#pragma once
#include "natives.hpp"
#include "vehicle.hpp"

namespace big::entity
{
	inline bool delete_entity(Entity& ent)
	{
		Entity temp = ent;

		if (!(ent && ENTITY::DOES_ENTITY_EXIST(ent)))
		{
			g_notification_service.push_error("Deletion failed", std::format("Entity does not exist {}", ent));
			return true;
		}

		if (ENTITY::IS_ENTITY_A_VEHICLE(ent))
		{
			// if (*g_pointers->m_gta.m_is_session_started && vehicle::is_player_veh(ent))
			// {
			// 	g_notification_service.push_error("Deletion failed", "It is a personal Vehicle");
			// 	return false;
			// }

			if (!vehicle::clear_all_peds(ent))
			{
				g_notification_service.push_error("Deletion failed", std::format("vehicle {} is not empty.", ent));
				return false;
			}
		}

		if (!take_control_of(ent))
		{
			g_notification_service.push_error("Deletion failed", std::format("failed to take_control_of {}", ent));
			return false;
		}

		if (ENTITY::IS_ENTITY_ATTACHED(ent))
			ENTITY::DETACH_ENTITY(ent, 0, 0);

		ENTITY::SET_ENTITY_COORDS_NO_OFFSET(ent, 7000.f, 7000.f, 15.f, 0, 0, 0);

		if (!ENTITY::IS_ENTITY_A_MISSION_ENTITY(ent))
			ENTITY::SET_ENTITY_AS_MISSION_ENTITY(ent, true, true);


		ENTITY::DELETE_ENTITY(&ent);

		// if (ENTITY::DOES_ENTITY_EXIST(ent))
		// 	ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&ent);

		script::get_current()->yield(150ms);

		if (ENTITY::DOES_ENTITY_EXIST(temp))
		{
			g_notification_service.push_error("Deletion failed", std::format("Entity {} exists even after deleting", temp));
			ent = temp;
			return false;
		}

		return true;
	}
}