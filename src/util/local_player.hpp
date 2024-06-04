#pragma once
#include "core/vars.hpp"
#include "gta/joaat.hpp"
#include "natives.hpp"
#include "services/players/player_service.hpp"
#include "util/player.hpp"

namespace big::local_player
{
	static int character_slot;

	inline int get_active_character_slot() // also known as get_character_index in lua yimmenu
	{
		STATS::STAT_GET_INT("MPPLY_LAST_MP_CHAR"_J, &character_slot, true);
		return character_slot;
	}

	inline std::string get_mp_prefix()
	{
		return "MP" + std::to_string(get_active_character_slot()) + "_";
	}

	inline void remove_wanted()
	{
		if (g_local_player && g_local_player->m_player_info->m_is_wanted)
		{
			g_local_player->m_player_info->m_wanted_level = 0;
			g_local_player->m_player_info->m_is_wanted    = false;
		}
	}

	inline bool is_in_cutscene()
	{
		if (g_local_player && g_local_player->m_player_info)
			return g_local_player->m_player_info->m_game_state == eGameState::InMPCutscene;
		return false;
	}

	inline bool is_in_interior()
	{
		int id = 0;
		if (auto self_ptr = g_player_service->get_self(); self_ptr->is_valid())
			id = self_ptr->id();
		return get_interior_from_player(id) != 0;
	}
}