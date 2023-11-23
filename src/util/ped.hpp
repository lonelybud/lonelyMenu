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

	inline void clean_ped()
	{
		Ped player_ped = ped::get_self_ped();

		PED::CLEAR_PED_BLOOD_DAMAGE(player_ped);
		PED::CLEAR_PED_ENV_DIRT(player_ped);
		PED::RESET_PED_VISIBLE_DAMAGE(player_ped);
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
