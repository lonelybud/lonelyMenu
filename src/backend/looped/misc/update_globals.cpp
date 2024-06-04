#include "backend/looped/looped.hpp"
#include "core/vars.hpp"
#include "gta/joaat.hpp"
#include "gta_util.hpp"
#include "natives.hpp"

namespace big
{
	void looped::update_globals()
	{
		auto network_player_mgr = gta_util::get_network_player_mgr();

		if (!network_player_mgr || !network_player_mgr->m_local_net_player || network_player_mgr->m_local_net_player->m_player_id == -1)
			self::id = 0;
		else
			self::id = network_player_mgr->m_local_net_player->m_player_id;

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
		}

		is_maintransition_script_active = SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH("maintransition"_J) != 0;
	}
}