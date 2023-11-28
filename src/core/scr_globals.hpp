#pragma once
#include "script_global.hpp"
#include "script_local.hpp"

namespace big::scr_globals
{
	static inline const script_global gsbd(2648711);
	static inline const script_global gsbd_kicking(1885447);

	static inline const script_global globalplayer_bd(2657704);
	static inline const script_global gpbd_fm_3(1895156);
	static inline const script_global gpbd_fm_1(1853988);

	static inline const script_global transition_state(1574996);
	static inline const script_global vehicle_global(1586488);
	static inline const script_global freemode_global(2794162);
	static inline const script_global transaction_overlimit(20483);
	static inline const script_global passive(1574582);
}

namespace big::scr_locals
{
	namespace am_hunt_the_beast
	{
		constexpr static auto broadcast_idx        = 600;
		constexpr static auto player_broadcast_idx = 2584;
	}
}