#pragma once
#include "rage/joaat.hpp"

namespace big
{
	inline const std::unordered_set<int> very_bad_script_ids = {
	    9 /*AM_Darts*/,
	    17 /*AM_PI_MENU*/,
	    20 /*fm_intro*/,
	    215 /*Pilot_School_MP*/,
	    224 /*am_darts_apartment*/,
	    226 /*grid_arcade_cabinet*/,
	    227 /*scroll_arcade_cabinet*/,
	    228 /*example_arcade*/,
	    229 /*road_arcade*/,
	    230 /*gunslinger_arcade*/,
	    231 /*wizard_arcade*/,
	    235 /*ggsm_arcade*/,
	    236 /*puzzle*/,
	    237 /*camhedz_arcade*/,
	    238 /*SCTV*/,
	};

	inline const std::unordered_set<int> bad_script_ids = {
	    212 /*golf_mp*/,
	    214 /*tennis_network_mp*/,
	    216 /*FM_Impromptu_DM_Controler*/,
	    217 /*fm_Bj_race_controler*/,
	    218 /*fm_deathmatch_controler*/,
	    221 /*FM_Race_Controler*/,
	    222 /*FM_Horde_Controler*/,
	};

	// inline constexpr auto bad_script_hashes = { // use it for intellisense
	inline const std::unordered_set<rage::joaat_t> bad_script_hashes = {
	    "am_darts"_J,
	    "am_darts_apartment"_J,
	    "example_arcade"_J,
	    "ggsm_arcade"_J,
	    "camhedz_arcade"_J,
	    "wizard_arcade"_J,
	    "puzzle"_J,
	    "fm_intro"_J,
	    "pilot_school_mp"_J,
	    "golf_mp"_J,
	    "tennis_network_mp"_J,
	    "fm_race_controler"_J,
	    "fm_horde_controler"_J,
	    "fm_mission_controller"_J,
	    "fm_mission_controller_2020"_J,
	    "fm_impromptu_dm_controler"_J,
	    "fm_deathmatch_controler"_J,
	    "fm_bj_race_controler"_J,
	    "fm_survival_controller"_J,
	    "sctv"_J,
	    "am_pi_menu"_J,
	    "scroll_arcade_cabinet"_J,
	    "grid_arcade_cabinet"_J,
	    "degenatron_games"_J,
	    "gunslinger_arcade"_J,
	    "road_arcade"_J,
	    "AM_MP_DRONE"_J,
	};
}
