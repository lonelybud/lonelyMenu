#pragma once
#include "gta/net_game_event.hpp"
#include "gta/net_object_mgr.hpp"
#include "natives.hpp"
#include "packet.hpp"
#include "services/players/player_service.hpp"
#include "vehicle.hpp"

#include <entities/CDynamicEntity.hpp>

namespace big::entity
{
	static int get_next_token_value(int prev_token)
	{
		for (int i = 0; i < 0x1F; i++)
		{
			if ((i << 27) - (prev_token << 27) > 0)
				return i;
		}

		return 0;
	}

	inline void force_remove_network_entity(rage::CDynamicEntity* entity)
	{
		std::uint16_t net_id = entity->m_net_object->m_object_id;
		int ownership_token  = entity->m_net_object->m_ownership_token;

		char buf[0x200]{};
		rage::datBitBuffer remove_buf(buf, sizeof(buf));
		int msgs_written = 0;

		if (ownership_token != -1)
		{
			remove_buf.Write<std::uint16_t>(net_id, 13);
			remove_buf.Write<int>(get_next_token_value(ownership_token), 5);
			msgs_written++;
		}
		else
		{
			// try all tokens if we don't know it
			for (int i = 0; i < 0x1F; i++)
			{
				remove_buf.Write<std::uint16_t>(net_id, 13);
				remove_buf.Write<int>(i, 5);
				msgs_written++;
			}
		}

		packet pack;
		pack.write_message(rage::eNetMessage::MsgPackedReliables);
		pack.write<int>(4, 4); // remove
		pack.write<int>(msgs_written, 5);
		pack.write<int>(remove_buf.GetPosition(), 13);
		pack.m_buffer.WriteArray(&buf, remove_buf.GetPosition());

		for (auto& player : g_player_service->players())
			if (player.second->is_valid() && (!player.second->get_ped() || player.second->get_ped()->m_net_object->m_object_id != net_id))
				pack.send(player.second->get_net_game_player()->m_msg_id);

		// delete locally
		if (auto object = (*g_pointers->m_gta.m_network_object_mgr)->find_object_by_id(net_id, true))
			(*g_pointers->m_gta.m_network_object_mgr)->UnregisterNetworkObject(object, 8, true, true);
	}

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
			// if (*g_pointers->m_gta.m_is_session_started && vehicle::check_if_player_veh(ent))
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