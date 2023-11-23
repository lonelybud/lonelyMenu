#pragma once
#include "core/scr_globals.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "script_global.hpp"

#include <script/globals/GPBD_FM_3.hpp>
#include <script/globals/GlobalPlayerBD.hpp>

namespace big::globals
{
	inline Interior get_interior_from_player(Player player)
	{
		return scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[player].CurrentInteriorIndex;
	}
}
