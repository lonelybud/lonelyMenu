#include "backend/looped/looped.hpp"
#include "gta_util.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "services/players/player_service.hpp"
#include "util/player.hpp"

#include <network/CNetworkPlayerMgr.hpp>

namespace big
{
	void looped::update_globals()
	{
		static bool last_dead = false;

		if (!*g_pointers->m_gta.m_network_player_mgr || !(*g_pointers->m_gta.m_network_player_mgr)->m_local_net_player
		    || (*g_pointers->m_gta.m_network_player_mgr)->m_local_net_player->m_player_id == -1)
			self::id = 0;
		else
			self::id = (*g_pointers->m_gta.m_network_player_mgr)->m_local_net_player->m_player_id;

		g_local_player = gta_util::get_local_ped();

		auto m_game_session_state = gta_util::get_network()->m_game_session_state;

		if (m_game_session_state == 0 || m_game_session_state > 4)
		{
			self::ped = PLAYER::PLAYER_PED_ID();

			self::pos = ENTITY::GET_ENTITY_COORDS(self::ped, 0);

			if (PED::IS_PED_IN_ANY_VEHICLE(self::ped, 0))
			{
				if (!self::veh)
					self::veh = PED::GET_VEHICLE_PED_IS_IN(self::ped, 0);
			}
			else
				self::veh = 0;

			if (g_local_player && g_local_player->m_player_info->m_game_state == eGameState::Died)
			{
				if (!last_dead)
				{
					auto ent = PED::GET_PED_SOURCE_OF_DEATH(self::ped);

					if (ent)
					{
						last_dead         = true;
						player_ptr player = nullptr;

						if (ENTITY::IS_ENTITY_A_PED(ent))
						{
							if (PED::IS_PED_A_PLAYER(ent))
								player = get_player_from_ped(ent);
						}
						else if (ENTITY::IS_ENTITY_A_VEHICLE(ent))
						{
							auto veh_ped = VEHICLE::GET_PED_IN_VEHICLE_SEAT(ent, -1, 0);

							if (PED::IS_PED_A_PLAYER(veh_ped))
								player = get_player_from_ped(veh_ped);
						}

						if (player)
							LOG(WARNING) << "You got Killed by: " << player->get_name();
					}
				}
			}
			else if (last_dead)
				last_dead = false;
		}
	}
}
