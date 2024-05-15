#include "core/data/misc.hpp"
#include "core/data/reactions.hpp"
#include "fiber_pool.hpp"
#include "gta/joaat.hpp"
#include "gta/net_game_event.hpp"
#include "hooking/hooking.hpp"
#include "services/notifications/notification_service.hpp"
#include "services/players/player_service.hpp"
#include "util/entity.hpp"
#include "util/globals.hpp"
#include "util/misc.hpp"
#include "util/vehicle.hpp"

#include <entities/CDynamicEntity.hpp>
#include <script/globals/GPBD_FM.hpp>
#include <script/globals/GlobalPlayerBD.hpp>

namespace big
{
	static inline bool is_invincible(big::player_ptr player)
	{
		if (auto ped = player->get_ped())
			return ped->m_damage_bits & (uint32_t)eEntityProofs::GOD || ped->m_damage_bits & (uint32_t)eEntityProofs::BULLET || ped->m_damage_bits & (uint32_t)eEntityProofs::EXPLOSION;

		return false;
	}
	static inline bool is_invincible_veh(big::player_ptr player)
	{
		auto vehicle = player->get_current_vehicle();
		return vehicle && vehicle->m_driver == player->get_ped() && (vehicle->m_damage_bits & (uint32_t)eEntityProofs::GOD);
	}

	static inline bool is_invisible(big::player_ptr player)
	{
		if (!player->get_ped())
			return false;

		if (scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[player->id()].IsInvisible)
			return true;

		if ((player->get_current_vehicle() && player->get_current_vehicle()->m_driver == player->get_ped())
		    || PLAYER::IS_REMOTE_PLAYER_IN_NON_CLONED_VEHICLE(player->id()))
			return false; // can't say

		return false; // TODO! have to get data from CPhysicalGameStateDataNode
		              //return (player->get_ped()->m_flags & (int)rage::fwEntity::EntityFlags::IS_VISIBLE) == 0;
	}

	static inline bool is_hidden_from_player_list(big::player_ptr player)
	{
		return scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[player->id()].CayoPericoFlags & 1;
	}

	static inline bool is_using_orbital_cannon(big::player_ptr player)
	{
		return scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[player->id()].OrbitalBitset.IsSet(eOrbitalBitset::kOrbitalCannonActive);
	}

	rage::CEventNetwork* hooks::get_network_event_data(int64_t unk, rage::CEventNetwork* net_event)
	{
		switch (net_event->get_type())
		{
		case rage::eEventNetworkType::CEventNetworkRemovedFromSessionDueToComplaints:
		{
			g_notification_service.push_warning("Kicked", "You have been desync kicked.", true);
			break;
		}
		case rage::eEventNetworkType::CEventNetworkEntityDamage:
		{
			rage::sEntityDamagedData damage_data;
			net_event->get_extra_information(&damage_data, sizeof(damage_data));

			if (auto damager = reinterpret_cast<CPed*>(g_pointers->m_gta.m_handle_to_ptr(damage_data.m_damager_index));
			    damager && damager->m_entity_type == 4 && damager->m_player_info && damage_data.m_victim_destroyed)
				if (auto victim = reinterpret_cast<CPed*>(g_pointers->m_gta.m_handle_to_ptr(damage_data.m_victim_index));
				    victim && victim->m_entity_type == 4 && damager != victim && victim->m_player_info)
					if (auto player = g_player_service->get_by_host_token(damager->m_player_info->m_net_player_data.m_host_token))
					{
						auto victim_player = victim == g_local_player ?
						    g_player_service->get_self() :
						    g_player_service->get_by_host_token(victim->m_player_info->m_net_player_data.m_host_token);

						if (victim == g_local_player)
							g_fiber_pool->queue_job([player] {
								std::string str = "You got Killed by: " + std::string(player->m_name);

								if (g_local_player->m_vehicle)
								{
									auto veh = g_pointers->m_gta.m_ptr_to_handle(g_local_player->m_vehicle);
									if (!vehicle::is_player_veh(veh) && entity::take_control_of(g_local_player->m_vehicle))
									{
										g_local_player->m_vehicle->m_door_lock_status = (int)eVehicleLockState::VEHICLELOCK_LOCKED;
										str += " (Veh Locked)";
									}
								}

								LOG(WARNING) << str;
							});
						else if (g_misc.notify_friend_killed && victim_player->is_friend())
							g_notification_service.push_warning("Friend Killed",
							    std::format("{} killed {}", player->m_name, victim_player->m_name),
							    true);

						victim_player->last_killed_by = player;

						if (is_using_orbital_cannon(player))
						{
							if (globals::get_interior_from_player(player->id()) == 0)
								g_reactions.Killed_with_orbital.process(player, victim_player);
						}
						else if (is_invincible(player))
							g_reactions.killed_with_god.process(player, victim_player);
						else if (is_invincible_veh(player))
							g_reactions.killed_with_veh_god.process(player, victim_player);

						if (is_hidden_from_player_list(player))
							g_reactions.killed_when_hidden.process(player, victim_player);
					}

			break;
		}
		}

		return g_hooking->get_original<get_network_event_data>()(unk, net_event);
	}
}
