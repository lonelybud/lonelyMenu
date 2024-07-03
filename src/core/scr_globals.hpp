#pragma once
#include "script_global.hpp"
#include "script_local.hpp"

namespace big::scr_globals
{
	inline const script_global gsbd(2648938);
	inline const script_global gsbd_kicking(1877042);
	inline const script_global gsbd_fm_events(1916617);
	inline const script_global globalplayer_bd(2657971);
	inline const script_global gpbd_fm_3(1887305);
	inline const script_global gpbd_fm_1(1845281);
	inline const script_global transition_state(1575011);
	inline const script_global vehicle_global(1586521);
	inline const script_global freemode_properties(2672855);
	inline const script_global freemode_global(2738934);
	inline const script_global transaction_overlimit(20913);
	inline const script_global session(1574589);
	inline const script_global session2(1575035);
	inline const script_global session3(32949);
	inline const script_global session4(1574942);
	inline const script_global session5(1575010);
	inline const script_global session6(2696130); // freemode -> if (NETWORK::NETWORK_IS_GAME_IN_PROGRESS() && !NETWORK::NETWORK_IS_ACTIVITY_SESSION())
	inline const script_global passive(1574582); // if (((!PED::IS_PED_IN_ANY_VEHICLE(PLAYER::GET_PLAYER_PED(bVar1), false) || Global_
	inline const script_global reset_clothing(104077); // freemode 75, &iLocal_.*, 2\);
}

namespace big::scr_locals
{
	namespace am_hunt_the_beast
	{
		constexpr static auto broadcast_idx        = 607;  // (bParam0) != 0;
		constexpr static auto player_broadcast_idx = 2588; // if (NETWORK::PARTICIPANT_ID_TO_INT() != -1)
	}
}