#pragma once
#include "services/bad_players/bad_players.hpp"
#include "services/notifications/notification_service.hpp"
#include "services/players/player.hpp"
#include "util/outfit.hpp"

namespace big
{
	inline std::string get_blocked_player_joined_log_string(big::player_ptr plyr)
	{
		if (auto net_data = plyr->get_net_data())
		{
			auto rockstar_id = net_data->m_gamer_handle.m_rockstar_id;
			auto player_name = net_data->m_name;

			plyr->is_blocked = true;
			plyr->is_spammer = bad_players_nm::bad_players_list[rockstar_id].is_spammer;

			return std::format("A Blocked {} {} ({}) has joined.", plyr->is_spammer ? "Spammer" : "Player", player_name, rockstar_id);
		}

		return "";
	}

	inline void steal_player_outfit(big::player_ptr plyr)
	{
		if (plyr->is_valid())
		{
			auto target = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(plyr->id());

			if (ENTITY::GET_ENTITY_MODEL(target) != ENTITY::GET_ENTITY_MODEL(self::ped))
			{
				g_notification_service->push_error("Steal Oufit", "Model not same.", true);
				return;
			}

			outfit::components_t components;
			outfit::props_t props;

			set_self_comps_props(components, props, target);
		}
	}

	inline player_ptr get_player_from_ped(Ped ped)
	{
		for (auto& p : g_player_service->players())
			if (p.second->get_ped())
				if (p.second->get_ped() == g_pointers->m_gta.m_handle_to_ptr(ped))
					return p.second;

		return nullptr;
	}
}