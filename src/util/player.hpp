#pragma once
#include "core/scr_globals.hpp"
#include "services/players/player_service.hpp"
#include "util/mobile.hpp"
#include "util/outfit.hpp"
#include "util/time.hpp"

#include <script/globals/GPBD_FM_3.hpp>
#include <script/globals/GlobalPlayerBD.hpp>

namespace big
{
	inline player_ptr get_player_from_ped(Ped ped)
	{
		for (auto& p : g_player_service->players())
			if (p.second->get_ped())
				if (p.second->get_ped() == g_pointers->m_gta.m_handle_to_ptr(ped))
					return p.second;

		return nullptr;
	}

	inline void steal_player_outfit(big::player_ptr plyr, bool save = false)
	{
		Ped target;

		if (plyr->is_valid() && (target = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(plyr->id())))
		{
			if (ENTITY::GET_ENTITY_MODEL(target) != ENTITY::GET_ENTITY_MODEL(self::ped))
			{
				g_notification_service.push_error("Steal Oufit", "Model not same.", true);
				return;
			}

			if (save)
				outfit::save_outfit(target, std::format("{}_{}.json", plyr->m_name, get_current_time_in_mill()), "");
			else
				outfit::set_self_comps_props({}, {}, target);
		}
	}

	inline bool player_is_driver(player_ptr target_plyr)
	{
		if (!g_local_player->m_vehicle)
			return false;

		// player was last driver
		if (g_local_player->m_vehicle->m_last_driver && g_local_player->m_vehicle->m_last_driver == target_plyr->get_ped())
			return true;

		// you are driver
		if (g_local_player->m_vehicle->m_driver && g_local_player->m_vehicle->m_driver == g_local_player)
		{
			g_log.log_additional("player_is_driver 1");
			return false;
		}

		if (auto driver =
		        g_local_player->m_vehicle->m_driver ? g_local_player->m_vehicle->m_driver : g_local_player->m_vehicle->m_last_driver)
		{
			// driver is a player present in session
			if (driver->m_player_info && g_player_service->get_by_host_token(driver->m_player_info->m_net_player_data.m_host_token))
			{
				g_log.log_additional(std::format("player_is_driver 2, {}", driver->m_player_info->m_net_player_data.m_name));
				return driver == target_plyr->get_ped();
			}

			// driver exists but is not a valid player
			return true;
		}

		g_log.log_additional("player_is_driver 3");
		return false;
	}

	inline bool is_in_vehicle(CPed* ped, CVehicle* vehicle)
	{
		if (ped == vehicle->m_driver)
			return true;

		for (int i = 0; i < 15; i++)
			if (vehicle->m_passengers[i] == ped)
				return true;

		return false;
	}

	inline bool is_player_our_bodyguard(Player sender)
	{
		auto& boss_goon = scr_globals::gpbd_fm_3.as<GPBD_FM_3*>()->Entries[self::id].BossGoon;

		if (boss_goon.Boss != self::id)
			return false;

		for (int i = 0; i < boss_goon.Goons.Size; i++)
		{
			if (boss_goon.Goons[i] == sender)
			{
				g_log.log_additional(std::format("is_player_our_bodyguard {}", sender));
				return true;
			}
		}

		return false;
	}

	inline bool is_player_our_boss(Player id)
	{
		return id == scr_globals::gpbd_fm_3.as<GPBD_FM_3*>()->Entries[self::id].BossGoon.Boss;
	}

	inline bool is_player_same_team(Player player_id)
	{
		if (NETWORK::NETWORK_IS_ACTIVITY_SESSION())
			// mission
			return PLAYER::GET_PLAYER_TEAM(player_id) == PLAYER::GET_PLAYER_TEAM(self::id);
		else
		{
			auto boss_goon = &scr_globals::gpbd_fm_3.as<GPBD_FM_3*>()->Entries[self::id].BossGoon;

			if (boss_goon->Boss == -1)
				return false;

			if (boss_goon->Boss == player_id)
				return true;

			if (boss_goon->Boss != self::id)
				boss_goon = &scr_globals::gpbd_fm_3.as<GPBD_FM_3*>()->Entries[boss_goon->Boss].BossGoon; // get their structure

			// bypass some P2Cs
			for (int i = 0; i < boss_goon->Goons.Size; i++)
				if (boss_goon->Goons[i] == player_id)
					return true;

			return false;
		}
	}

	inline bool is_local_vehicle_net_id(int16_t net_id)
	{
		return g_local_player && g_local_player->m_vehicle && g_local_player->m_vehicle->m_net_object
		    && g_local_player->m_vehicle->m_driver == g_local_player && g_local_player->m_vehicle->m_net_object->m_object_id == net_id;
	}

	inline bool is_invincible(big::player_ptr player)
	{
		if (auto ped = player->get_ped())
			return ped->m_damage_bits & (uint32_t)eEntityProofs::GOD || ped->m_damage_bits & (uint32_t)eEntityProofs::BULLET || ped->m_damage_bits & (uint32_t)eEntityProofs::EXPLOSION;

		return false;
	}

	inline bool is_invincible_veh(big::player_ptr player)
	{
		auto vehicle = player->get_current_vehicle();
		return vehicle && vehicle->m_driver == player->get_ped() && (vehicle->m_damage_bits & (uint32_t)eEntityProofs::GOD);
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

	inline Interior get_interior_from_player(Player player)
	{
		return scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[player].CurrentInteriorIndex;
	}
}