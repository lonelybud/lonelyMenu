#pragma once
#include "services/notifications/notification_service.hpp"
#include "services/players/player.hpp"

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

	inline void set_self_comps_props(outfit::components_t& components, outfit::props_t& props)
	{
		for (auto item : components.items)
		{
			auto draw    = PED::GET_PED_DRAWABLE_VARIATION(target, item.id);
			auto texture = PED::GET_PED_TEXTURE_VARIATION(target, item.id);

			PED::SET_PED_COMPONENT_VARIATION(self::ped, item.id, draw, texture, PED::GET_PED_PALETTE_VARIATION(self::ped, item.id));
		}

		PED::CLEAR_ALL_PED_PROPS(self::ped, 0);

		for (auto& item : props.items)
		{
			auto draw    = PED::GET_PED_PROP_INDEX(target, item.id, 1);
			auto texture = PED::GET_PED_PROP_TEXTURE_INDEX(target, item.id);

			PED::SET_PED_PROP_INDEX(self::ped, item.id, draw, texture, TRUE, 1);
		}
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

			set_self_comps_props(components, props);
		}
	}
}