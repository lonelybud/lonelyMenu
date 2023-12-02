#include "backend/looped/looped.hpp"
#include "gta_util.hpp"
#include "natives.hpp"
#include "pointers.hpp"

#include <network/CNetworkPlayerMgr.hpp>

namespace big
{
	void looped::system_self_globals()
	{
		if (!*g_pointers->m_gta.m_network_player_mgr || !(*g_pointers->m_gta.m_network_player_mgr)->m_local_net_player
		    || (*g_pointers->m_gta.m_network_player_mgr)->m_local_net_player->m_player_id == -1)
			self::id = 0;
		else
			self::id = (*g_pointers->m_gta.m_network_player_mgr)->m_local_net_player->m_player_id;

		auto state = gta_util::get_network()->m_game_session_state;
		if (state == 0 || state > 4)
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
		}
	}
}
