#pragma once
#include "gta/joaat.hpp"

namespace big
{
	namespace script_block_options
	{
		const static std::unordered_set<std::uint32_t> arcade_games = {"am_mp_arcade_claw_crane"_J, "am_mp_arcade_fortune_teller"_J, "am_mp_arcade_love_meter"_J, "am_mp_arcade_strength_test"_J, "grid_arcade_cabine"_J, "gunslinger_arcade"_J, "road_arcade"_J, "dont_cross_the_line"_J, "camhedz_arcade"_J};
		const static std::unordered_set<std::uint32_t> casino_games = {"casinoroulette"_J, "casino_lucky_wheel"_J, "casino_slots"_J, "three_card_poker"_J, "blackjack"_J};
		const static std::unordered_set<std::uint32_t> interiors = {"am_mp_property_ext"_J, "am_mp_smpl_interior_ext"_J}; // blocking the actual interior scripts would softlock the game
		const static std::unordered_set<std::uint32_t> prostitutes = {"am_prostitute"_J, "pb_prostitute"_J};
		const static std::unordered_set<std::uint32_t> sitting = {"am_mp_boardroom_seating"_J, "arcade_seating"_J, "arena_box_bench_seats"_J, "arena_workshop_seats"_J, "auto_shop_seating"_J, "base_corridor_seats"_J, "base_entrance_seats"_J, "base_heist_seats"_J, "base_lounge_seats"_J, "base_quaters_seats"_J, "base_reception_seats"_J, "beach_exterior_seating"_J, "business_hub_garage_seats"_J, "car_meet_exterior_seating"_J, "casino_bar_seating"_J, "casino_exterior_seating"_J, "casino_interior_seating"_J, "casino_main_lounge_seating"_J, "casino_nightclub_seating"_J, "casino_penthouse_seating"_J, "fixer_hq_seating"_J, "fixer_hq_seating_op_floor"_J, "fixer_hq_seating_pq"_J, "juggalo_hideout_seating"_J, "multistorey_garage_ext_seating"_J, "multistorey_garage_seating"_J, "music_studio_seating"_J, "music_studio_seating_external"_J, "nightclub_ground_floor_seats"_J, "nightclub_office_seats"_J, "nightclub_vip_seats"_J, "salvage_yard_seating"_J, "simeon_showroom_seating"_J};
		const static std::unordered_set<std::uint32_t> sleeping = {"mp_bed_high"_J, "ob_mp_bed_high"_J, "ob_mp_bed_low"_J, "ob_mp_bed_med"_J};
		const static std::unordered_set<std::uint32_t> stores = {"clothes_shop_mp"_J, "hairdo_shop_mp"_J, "tattoo_shop"_J, "am_hold_up"_J, "ob_cashregister"_J};
		const static std::unordered_set<std::uint32_t> strip_club = {"stripclub_drinking"_J, "stripclub_mp"_J, "stripperhome"_J, "ob_mp_stripper"_J, "sclub_front_bouncer"_J};
		const static std::unordered_set<std::uint32_t> vending_machines = {"ob_vend1"_J, "ob_vend2"_J};
		const auto hash_lsc                                             = "carmod_shop"_J;
		const auto hash_ammunation                                      = "gunclub_shop"_J;
		const auto hash_drones                                          = "am_mp_drone"_J;
		const auto hash_impromptu_race                                  = "mg_race_to_point"_J;
		const auto hash_movies                                          = "act_cinema"_J;
		const auto hash_street_dealer                                   = "fm_street_dealer"_J;
		const auto hash_atms                                            = "atm_trigger"_J;
		const auto hash_impromptu_dm                                    = "fm_impromptu_dm_controler"_J;
		const auto hash_gang_attacks                                    = "fm_hideout_controler"_J;
	}

	struct script_types_options
	{
		bool arcade_games     = false;
		bool casino_games     = false;
		bool interiors        = false;
		bool prostitutes      = false;
		bool sitting          = false;
		bool sleeping         = false;
		bool stores           = false;
		bool strip_club       = false;
		bool vending_machines = false;
		bool lsc              = false;
		bool ammunation       = false;
		bool drones           = false;
		bool impromptu_race   = false;
		bool movies           = false;
		bool street_dealer    = false;
		bool atms             = false;
		bool impromptu_dm     = false;
		bool gang_attacks     = false;
	};
}
