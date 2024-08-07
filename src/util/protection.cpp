#include "protection.hpp"

#include "core/data/all_script_names.hpp"
#include "core/data/debug.hpp"
#include "model_info.hpp"
#include "natives.hpp"
#include "rage/joaat.hpp"

namespace big::protection
{
	static const std::unordered_set<rage::joaat_t> crash_objects = {
	    "prop_dummy_01"_J,
	    "prop_dummy_car"_J,
	    "prop_dummy_light"_J,
	    "prop_dummy_plane"_J,
	    "prop_distantcar_night"_J,
	    "prop_distantcar_day"_J,
	    "hei_bh1_08_details4_em_night"_J,
	    "dt1_18_sq_night_slod"_J,
	    "ss1_12_night_slod"_J,
	    "hash_b334b5e2_qyquzxq_collision"_J,
	    "h4_prop_bush_bgnvla_med_01"_J,
	    "h4_prop_bush_bgnvla_lrg_01"_J,
	    "h4_prop_bush_buddleia_low_01"_J,
	    "h4_prop_bush_ear_aa"_J,
	    "h4_prop_bush_ear_ab"_J,
	    "h4_prop_bush_fern_low_01"_J,
	    "h4_prop_bush_fern_tall_cc"_J,
	    "h4_prop_bush_mang_ad"_J,
	    "h4_prop_bush_mang_low_aa"_J,
	    "h4_prop_bush_mang_low_ab"_J,
	    "h4_prop_bush_seagrape_low_01"_J,
	    "prop_h4_ground_cover"_J,
	    "h4_prop_weed_groundcover_01"_J,
	    "h4_prop_grass_med_01"_J,
	    "h4_prop_grass_tropical_lush_01"_J,
	    "h4_prop_grass_wiregrass_01"_J,
	    "h4_prop_weed_01_plant"_J,
	    "h4_prop_weed_01_row"_J,
	    "urbanweeds02_l1"_J,
	    "proc_forest_grass01"_J,
	    "prop_small_bushyba"_J,
	    "v_res_d_dildo_a"_J,
	    "v_res_d_dildo_b"_J,
	    "v_res_d_dildo_c"_J,
	    "v_res_d_dildo_d"_J,
	    "v_res_d_dildo_e"_J,
	    "v_res_d_dildo_f"_J,
	    "v_res_skateboard"_J,
	    "prop_battery_01"_J,
	    "prop_barbell_01"_J,
	    "prop_barbell_02"_J,
	    "prop_bandsaw_01"_J,
	    "prop_bbq_3"_J,
	    "v_med_curtainsnewcloth2"_J,
	    "bh1_07_flagpoles"_J,
	    "hash_058a7eb5_deihiws_collision"_J,
	    "proc_dry_plants_01"_J,
	    "proc_leafyplant_01"_J,
	    "proc_grassplantmix_02"_J,
	    "proc_dryplantsgrass_01"_J,
	    "proc_dryplantsgrass_02"_J,
	    "proc_grasses01"_J,
	    "prop_dryweed_002_a"_J,
	    "prop_fernba"_J,
	    "prop_weed_001_aa"_J,
	    "urbangrnfrnds_01"_J,
	    "urbanweeds01"_J,
	    "prop_dandy_b"_J,
	    "v_proc2_temp"_J,
	    "prop_fernbb"_J,
	    "proc_drygrassfronds01"_J,
	    "prop_log_ae"_J,
	    "prop_grass_da"_J,
	    "prop_fragtest_cnst_04"_J,
	    "proc_drygrasses01"_J,
	    "proc_drygrasses01b"_J,
	    "proc_brittlebush_01"_J,
	    "proc_forest_ivy_01"_J,
	    "proc_grassdandelion01"_J,
	    "proc_grasses01b"_J,
	    "proc_grassfronds01"_J,
	    "proc_grassplantmix_01"_J,
	    "proc_indian_pbrush_01"_J,
	    "proc_leafybush_01"_J,
	    "proc_lizardtail_01"_J,
	    "proc_lupins_01"_J,
	    "proc_meadowmix_01"_J,
	    "proc_meadowpoppy_01"_J,
	    "proc_desert_sage_01"_J,
	    "prop_saplin_001_b"_J,
	    "prop_thindesertfiller_aa"_J,

	    // experimental
	    "urbanweeds02"_J,
	    "prop_weed_002_ba"_J,
	    "urbanweeds01_l1"_J,
	    "urbangrngrass_01"_J,
	    "urbandryfrnds_01"_J,
	};

	static const std::unordered_set<rage::joaat_t> crash_objects2 = {
	    "prop_facgate_05_r_dam_l1"_J,
	    "v_61_lng_mesh_unita_swap"_J,
	    "prop_crt_mon_02"_J,
	    "xs_prop_arena_pit_fire_01a_wl"_J,
	    "prop_billboard_07"_J,
	    "prop_recyclebin_02_d"_J,
	    "prop_billboard_14"_J,
	    "v_res_fa_book02"_J,
	    "sf_int1_dropdownlight041"_J,
	    "w_sg_pumpshotgunmk2_camo6"_J,
	    "ch_prop_cash_low_trolly_01b"_J,
	    "prop_ld_headset_01"_J,
	    "sf_int1_office_wpaper_6"_J,
	    "v_74_it3_ceiling_smoke_01_skin"_J,
	    "prop_sh_tt_fridgedoor"_J,
	    "ch_prop_arcade_race_02a"_J,
	    "v_61_kit_over_dec_cruma"_J,
	    "prop_snow_streetlight_01_frag_"_J,
	    "v_34_sm_proc"_J,
	    "tr_int1_lightcap_proxy001"_J,
	    "v_ilev_tort_door"_J,
	    "ch_prop_arcade_street_01b"_J,
	    "prop_ic_repair_p"_J,
	    "sf_int2_elevator_details_02"_J,
	    "prop_cs4_05_tdoor"_J,
	    "vfx_it1_02"_J,
	    "cloudhat_wispy_b"_J,
	    "hei_p_pre_heist_trash"_J,
	    "v_ilev_trev_patiodoor"_J,
	    "prop_bin_10a"_J,
	    "sm_prop_offchair_smug_01"_J,
	    "stt_prop_stunt_tube_fn_02"_J,
	    "m23_2_prop_m32_hat_captain_01a"_J,
	    "vw_prop_vw_arcade_04d_screen"_J,
	    "prop_food_juice02"_J,
	    "m23_2_prop_m32_mazebankcard_01a"_J,
	    "v_28_alrm_case011"_J,
	    "apa_mp_h_acc_pot_pouri_01"_J,
	    "xm3_prop_xm3_crate_ammo_01a"_J,
	    "prop_blackjack_01"_J,
	    "prop_cs_mini_tv"_J,
	    "v_res_fa_basket"_J,
	    "prop_plant_int_02b"_J,
	    "prop_umpire_01"_J,
	    "gr_prop_gr_fnclink_03g"_J,
	    "prop_rub_tyre_dam3"_J,
	    "ba_prop_battle_lights_support"_J,
	    "prop_fnclink_09a"_J,
	    "ba_prop_battle_dj_mixer_01e"_J,
	    "ar_prop_ar_neon_gate8x_04a"_J,
	    "xs_propintarena_structure_f_03b"_J,
	    "des_plog_door_end"_J,
	    "v_ret_fh_chair01"_J,
	    "gr_prop_gr_offchair_01a"_J,
	    "hei_p_pre_heist_coke"_J,
	    "cloudhat_puff_b"_J,
	    "v_ind_cfbucket"_J,
	    "p_hw1_22_doors_s"_J,
	    "arbitergt"_J,
	    "slod_human"_J,
	    "proc_stones_06"_J,
	    "v_corp_conftable2"_J,
	    "ng_proc_temp"_J,
	    "v_corp_cd_wellies"_J,
	    "v_res_rosevase"_J,
	    "v_corp_filecabdark03"_J,
	    "v_res_tt_porndvd01"_J,
	    "v_res_tt_porndvd02"_J,
	    "v_res_tt_porndvd03"_J,
	    "v_res_tt_porndvd04"_J,
	    "v_corp_lazychair"_J,
	    "v_corp_go_glass2"_J,
	    "v_serv_ct_chair01"_J,
	    "v_corp_deskdrawfd"_J,
	    "prop_saplin_002_b"_J,
	    // "urbangrngrass_01"_J,
	    "v_res_fa_mag_motor"_J,
	    "v_corp_cashpack"_J,
	    "v_corp_lowcabdark01"_J,
	    // "prop_weed_002_ba"_J,
	    "prop_saplin_002_c"_J,
	    "proc_sml_reeds_01b"_J,
	    // "urbanweeds01_l1"_J,
	    "prop_grass_dry_02"_J,
	    "v_corp_fib_glass1"_J,
	    "v_serv_bs_clutter"_J,
	    "proc_sml_reeds_01c"_J,
	    "v_res_tt_pharm3"_J,
	    "v_res_m_h_sofa"_J,
	    "v_serv_gt_glass1"_J,
	    "prop_log_ad"_J,
	    "prop_grass_dry_03"_J,
	    "prop_grass_ca"_J,
	    "v_res_tt_pharm2"_J,
	    "prop_log_ac"_J,
	    "v_serv_ct_monitor02"_J,
	    "v_corp_offchairfd"_J,
	    "v_res_tt_pharm10"_J,
	    "proc_scrub_bush01"_J,
	    "v_corp_closed_sign"_J,
	    "v_res_kitchnstool"_J,
	    "v_serv_firealarm"_J,
	    "urbandrygrass_01"_J,
	    "v_corp_filecabdark01"_J,
	    "prop_flowerweed_005_a"_J,
	    "v_corp_lngestool"_J,
	    "v_corp_maindeskfd"_J,
	    "v_res_m_l_chair1"_J,
	    "v_res_m_spanishbox"_J,
	    "v_serv_gt_glass2"_J,
	    "v_corp_bk_lflts"_J,
	    "v_corp_lazychairfd"_J,
	    "prop_tall_grass_ba"_J,
	    "sm_hanger_portacabin"_J,
	    "v_corp_humidifier"_J,
	    "v_corp_cashtrolley_2"_J,
	    "v_corp_bk_lamp1"_J,
	    "v_corp_bank_pen"_J,
	    "prop_saplin_001_c"_J,
	    "prop_tall_drygrass_aa"_J,
	    "v_serv_ct_monitor01"_J,
	    "v_corp_filecabtall_01"_J,
	    "proc_wildquinine"_J,
	    "v_corp_facebeanbagd"_J,
	    "v_serv_bs_foamx3"_J,
	    "v_corp_bk_rolladex"_J,
	    "v_corp_facebeanbagc"_J,
	    "v_corp_banktrolley"_J,
	    "v_corp_bk_lfltstand"_J,
	    "proc_stones_01"_J,
	    "v_corp_fib_glass_thin"_J,
	    "v_serv_ct_monitor03"_J,
	    "prop_log_ab"_J,
	    "v_corp_facebeanbagb"_J,
	    "v_serv_bs_cond"_J,
	    "v_corp_bk_lamp2"_J,
	    // "urbandryfrnds_01"_J,
	    "v_corp_filecablow"_J,
	    "v_serv_bs_gelx3"_J,
	    "v_corp_bk_pens"_J,
	    "prop_log_aa"_J,
	    "v_corp_deskseta"_J,
	    "v_corp_facebeanbag"_J,
	    "proc_stones_02"_J,
	    "proc_sage_01"_J,
	    "v_corp_fleeca_display"_J,
	    "proc_sml_reeds_01"_J,
	    "v_serv_ct_monitor04"_J,
	    "v_res_fa_plant01"_J,
	    "v_serv_ct_binoculars"_J,
	    "proc_stones_03"_J,
	    "v_corp_deskdrawdark01"_J,
	    "v_serv_ct_monitor05"_J,
	    "v_corp_filecabdark02"_J,
	    "v_corp_desksetb"_J,
	    "prop_grass_001_a"_J,
	    "v_corp_lidesk01"_J,
	    "v_corp_maindesk"_J,
	    "proc_stones_04"_J,
	    "v_corp_lngestoolfd"_J,
	    "v_serv_ct_monitor06"_J,
	    "v_serv_ct_lamp"_J,
	    "v_corp_hicksdoor"_J,
	    "prop_log_af"_J,
	    "v_res_tt_cbbox"_J,
	    "sm_hanger_mod_cabin1"_J,
	    "sm_hanger_office_traditional"_J,
	    "proc_stones_05"_J,
	    "v_serv_ct_chair02"_J,
	    "v_corp_filecabtall"_J,
	    "v_serv_ct_monitor07"_J,

	    //
	    "proc_forest_grass_01"_J,
	    "prop_fern_01"_J,
	    "prop_dryweed_001_a"_J,
	    // "urbanweeds02"_J,
	    "urbanweeds02_l2"_J,

	    // vehicles
	    "barracks"_J,
	    "barracks3"_J,
	    "dune"_J,
	    "marquis"_J,
	    "marshall"_J,
	    "monster"_J,
	    "tug"_J,
	};

	bool is_crash_object(rage::joaat_t model, player_ptr p)
	{
		if (!model_info::get_model(model))
			return false;

		if (crash_objects.contains(model))
		{
			g_log.log_additional(std::format("crash_objects {}, {}", model, p ? p->m_name : ""));
			return true;
		}

		if (crash_objects2.contains(model))
			g_log.log_additional(std::format("crash_objects2 {}, {}", model, p ? p->m_name : ""));

		if (!model_info::is_model_of_type(model, eModelType::Object, eModelType::Time, eModelType::Weapon, eModelType::Destructable, eModelType::WorldObject, eModelType::Sprinkler, eModelType::Unk65, eModelType::Plant, eModelType::LOD, eModelType::Unk132, eModelType::Building))
			return true;

		return false;
	}

	static const std::unordered_set<rage::joaat_t> crash_peds = {"slod_human"_J, "slod_small_quadped"_J, "slod_large_quadped"_J};
	bool is_crash_ped(rage::joaat_t model)
	{
		if (crash_peds.contains(model))
			return true;
		if (!model_info::is_model_of_type(model, eModelType::Ped, eModelType::OnlineOnlyPed))
			return true;
		return false;
	}

	static const std::unordered_set<rage::joaat_t> crash_vehicles = {"arbitergt"_J, "astron2"_J, "cyclone2"_J, "ignus2"_J, "s95"_J};
	bool is_crash_vehicle(rage::joaat_t model)
	{
		if (crash_vehicles.contains(model))
			return true;
		if (!model_info::is_model_of_type(model, eModelType::Vehicle, eModelType::Unk133))
			return true;
		return false;
	}

	static const std::unordered_set<rage::joaat_t> cage_objects = {"stt_prop_stunt_tube_s"_J, "prop_fnclink_03e"_J, "prop_gold_cont_01"_J, "prop_gold_cont_01b"_J, "prop_rub_cage01a"_J};
	bool is_cage_object(rage::joaat_t model)
	{
		if (cage_objects.contains(model))
			return true;

		// https://github.com/Deadlineem/Extras-Addon-for-YimMenu/blob/main/Extras-Addon.lua
		// Safe cage, 420 Cage
		if (model == "p_v_43_safe_s"_J || model == "bkr_prop_weed_lrg_01a"_J)
			return true;

		return false;
	}

	static const std::unordered_set<rage::joaat_t> valid_player_models = {
	    "mp_m_freemode_01"_J,
	    "mp_f_freemode_01"_J,
	    "u_m_m_filmdirector"_J,
	    "player_zero"_J,
	    "player_one"_J,
	    "player_two"_J,
	    // peyote
	    "A_C_Boar"_J,
	    "A_C_Cat_01"_J,
	    "A_C_Cow"_J,
	    "A_C_Coyote"_J,
	    "A_C_Deer"_J,
	    "A_C_Husky"_J,
	    "A_C_MtLion"_J,
	    "A_C_Pig"_J,
	    "A_C_Poodle"_J,
	    "A_C_Pug"_J,
	    "A_C_Rabbit_01"_J,
	    "A_C_Retriever"_J,
	    "A_C_Rottweiler"_J,
	    "A_C_shepherd"_J,
	    "A_C_Westy"_J,
	    "A_C_Chickenhawk"_J,
	    "A_C_Cormorant"_J,
	    "A_C_Crow"_J,
	    "A_C_Hen"_J,
	    "A_C_Pigeon"_J,
	    "A_C_Seagull"_J,
	    "A_C_Dolphin"_J,
	    "A_C_Fish"_J,
	    "A_C_KillerWhale"_J,
	    "A_C_SharkHammer"_J,
	    "A_C_SharkTiger"_J,
	    "A_C_Stingray"_J,
	    "IG_Orleans"_J,
	    "A_C_Chop"_J,
	    "A_C_HumpBack"_J,
	};
	bool is_valid_player_model(rage::joaat_t model)
	{
		return valid_player_models.contains(model);
	}

	enum class script_block_state
	{
		BLOCK_ALWAYS,
		BLOCK_IN_FREEMODE
	};

	static std::unordered_map<rage::joaat_t, script_block_state> script_block_states = {
	    {"AM_Darts"_J, script_block_state::BLOCK_ALWAYS},
	    {"AM_PI_MENU"_J, script_block_state::BLOCK_ALWAYS},
	    {"fm_intro"_J, script_block_state::BLOCK_ALWAYS},
	    {"golf_mp"_J, script_block_state::BLOCK_IN_FREEMODE},
	    {"tennis_network_mp"_J, script_block_state::BLOCK_IN_FREEMODE},
	    {"Pilot_School_MP"_J, script_block_state::BLOCK_ALWAYS},
	    {"FM_Impromptu_DM_Controler"_J, script_block_state::BLOCK_IN_FREEMODE},
	    {"fm_deathmatch_controler"_J, script_block_state::BLOCK_IN_FREEMODE},
	    {"FM_Race_Controler"_J, script_block_state::BLOCK_IN_FREEMODE},
	    {"FM_Horde_Controler"_J, script_block_state::BLOCK_IN_FREEMODE},
	    {"am_darts_apartment"_J, script_block_state::BLOCK_ALWAYS},
	    {"grid_arcade_cabinet"_J, script_block_state::BLOCK_ALWAYS},
	    {"scroll_arcade_cabinet"_J, script_block_state::BLOCK_ALWAYS},
	    {"example_arcade"_J, script_block_state::BLOCK_ALWAYS},
	    {"road_arcade"_J, script_block_state::BLOCK_ALWAYS},
	    {"gunslinger_arcade"_J, script_block_state::BLOCK_ALWAYS}, // Badlands Revenge II?
	    {"wizard_arcade"_J, script_block_state::BLOCK_ALWAYS},
	    {"ggsm_arcade"_J, script_block_state::BLOCK_ALWAYS}, // Space Monkey?
	    {"puzzle"_J, script_block_state::BLOCK_ALWAYS},      // Qub3d?
	    {"camhedz_arcade"_J, script_block_state::BLOCK_ALWAYS},
	    {"SCTV"_J, script_block_state::BLOCK_ALWAYS},
	};

	bool should_allow_script_launch(int launcher_script)
	{
		if (launcher_script >= launcher_scripts.size())
			return false;

		auto script = launcher_scripts[launcher_script];

		if (auto it = script_block_states.find(script); it != script_block_states.end())
		{
			if (it->second == script_block_state::BLOCK_ALWAYS)
				return false;

			if (it->second == script_block_state::BLOCK_IN_FREEMODE && !NETWORK::NETWORK_IS_ACTIVITY_SESSION())
				return false;
		}

		return true;
	}
}
