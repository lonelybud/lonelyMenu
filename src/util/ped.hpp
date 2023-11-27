#pragma once
#include "gta/enums.hpp"
#include "math.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "services/players/player_service.hpp"

namespace big::ped
{
	inline Ped get_self_ped()
	{
		return PLAYER::PLAYER_PED_ID();
	}
	inline Vector3 get_self_pos(Ped ped)
	{
		return ENTITY::GET_ENTITY_COORDS(ped ? ped : get_self_ped(), 0);
	}

	inline player_ptr get_player_from_ped(Ped ped)
	{
		for (auto& p : g_player_service->players())
		{
			if (p.second->get_ped())
			{
				if (p.second->get_ped() == g_pointers->m_gta.m_handle_to_ptr(ped))
					return p.second;
			}
		}
		return nullptr;
	}
}
