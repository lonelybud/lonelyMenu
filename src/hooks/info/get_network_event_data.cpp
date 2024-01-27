#include "core/data/misc.hpp"
#include "core/data/reactions.hpp"
#include "gta/joaat.hpp"
#include "gta/net_game_event.hpp"
#include "hooking/hooking.hpp"
#include "services/notifications/notification_service.hpp"
#include "services/players/player_service.hpp"
#include "util/globals.hpp"
#include "util/misc.hpp"

#include <entities/CDynamicEntity.hpp>
#include <script/globals/GPBD_FM.hpp>
#include <script/globals/GlobalPlayerBD.hpp>

namespace big
{
	inline bool is_invincible(big::player_ptr player)
	{
		return player->get_ped() && (player->get_ped()->m_damage_bits & (1 << 8));
	}

	inline bool is_invisible(big::player_ptr player)
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

	inline bool is_hidden_from_player_list(big::player_ptr player)
	{
		return scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[player->id()].CayoPericoFlags & 1;
	}

	inline bool is_using_orbital_cannon(big::player_ptr player)
	{
		return scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[player->id()].OrbitalBitset.IsSet(eOrbitalBitset::kOrbitalCannonActive);
	}

	rage::CEventNetwork* hooks::get_network_event_data(int64_t unk, rage::CEventNetwork* net_event)
	{
		switch (net_event->get_type())
		{
		case rage::eEventNetworkType::CEventNetworkRemovedFromSessionDueToComplaints:
		{
			g_notification_service->push_warning("Kicked", "You have been desync kicked.", true);
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
						if (victim == g_local_player)
						{
							LOG(WARNING) << "You got Killed by: " << player->get_name();
							g_player_service->get_self()->last_killed_by = player;
						}
						else if (auto victim_player =
						             g_player_service->get_by_host_token(victim->m_player_info->m_net_player_data.m_host_token))
						{
							victim_player->last_killed_by = player;

							if (g_misc.notify_friend_killed && victim_player && victim_player->is_friend())
								g_notification_service->push_warning("Friend Killed",
								    std::format("{} killed {}", player->get_name(), victim_player->get_name()),
								    true);
						}

						if (is_invincible(player))
							g_reactions.killed_with_god.process(player);

						if (is_invisible(player))
							g_reactions.killed_with_invis.process(player);

						if (is_hidden_from_player_list(player))
							g_reactions.killed_when_hidden.process(player);

						if (is_using_orbital_cannon(player) && globals::get_interior_from_player(player->id()) == 0)
							g_reactions.Killed_with_orbital.process(player);
					}

			break;
		}
		}

		return g_hooking->get_original<get_network_event_data>()(unk, net_event);
	}
}
