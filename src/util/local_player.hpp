#pragma once
#include "gta/joaat.hpp"
#include "natives.hpp"

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
}