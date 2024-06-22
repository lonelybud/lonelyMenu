#pragma once
#include "backend/reactions/reaction.hpp"
#include "core/enums.hpp"

namespace big
{
	inline struct g_reactions_t
	{
		// kicks
		reaction end_session_kick{reaction_type::kick_player, reaction_sub_type::end_session_kick, "End Session Kick", false, true, false, reaction_karma::kick_player};
		reaction network_bail{reaction_type::kick_player, reaction_sub_type::network_bail, "Network bail", false, true, false, reaction_karma::kick_player};
		reaction null_function_kick{reaction_type::kick_player, reaction_sub_type::null_function_kick, "Null Function Kick", false, true, false, reaction_karma::kick_player};
		reaction oom_kick{reaction_type::kick_player, reaction_sub_type::oom_kick, "OOM Kick (radio)", false, true, false, reaction_karma::kick_player};
		reaction oom_kick2{reaction_type::kick_player, reaction_sub_type::oom_kick2, "OOM Kick (host migr.)", false, true, false, reaction_karma::kick_player};
		reaction tse_freeze{reaction_type::kick_player, reaction_sub_type::tse_freeze, "TSE Freeze", false, true, false, reaction_karma::kick_player};

		// crashes
		reaction scripted_event_crash{reaction_type::crash_player, reaction_sub_type::scripted_event_crash, "Scripted Event Crash", false, true, false, reaction_karma::kick_player};
		reaction stand_user_crash{reaction_type::crash_player, reaction_sub_type::stand_user_crash, "Stand User Crash", false, true, false, reaction_karma::kick_player};
		reaction crash1{reaction_type::crash_player, reaction_sub_type::crash1, "Invalid vehicle model Crash", false, true, false, reaction_karma::kick_player};
		reaction crash2{reaction_type::crash_player, reaction_sub_type::crash2, "Vehicle model mismatch Crash", false, true, false, reaction_karma::infraction_based};
		reaction crash3{reaction_type::crash_player, reaction_sub_type::crash3, "Invalid door model Crash", false, true, false, reaction_karma::kick_player};
		reaction crash4{reaction_type::crash_player, reaction_sub_type::crash4, "Invalid pickup model Crash", false, true, false, reaction_karma::infraction_based};
		reaction crash5{reaction_type::crash_player, reaction_sub_type::crash5, "Invalid pickup weapon component hash Crash", false, true, false, reaction_karma::infraction_based};
		reaction crash6{reaction_type::crash_player, reaction_sub_type::crash6, "Infinite physical attachment crash", false, true, false, reaction_karma::kick_player};
		reaction crash7{reaction_type::crash_player, reaction_sub_type::crash7, "Invalid attachment Crash", false, true, false, reaction_karma::kick_player};
		reaction crash8{reaction_type::crash_player, reaction_sub_type::crash8, "Invalid ped model Crash", false, true, false, reaction_karma::none};
		reaction crash9{reaction_type::crash_player, reaction_sub_type::crash9, "Invalid ped prop model Crash", false, true, false, reaction_karma::infraction_based};
		reaction crash10{reaction_type::crash_player, reaction_sub_type::crash10, "Infinite ped attachment Crash", false, true, false, reaction_karma::infraction_based};
		reaction crash11{reaction_type::crash_player, reaction_sub_type::crash11, "Invalid object model Crash", false, true, false, reaction_karma::none};
		reaction crash12{reaction_type::crash_player, reaction_sub_type::crash12, "Invalid player model (appearance node) Crash", false, true, false, reaction_karma::kick_player};
		reaction crash13{reaction_type::crash_player, reaction_sub_type::crash13, "Invalid player model (creation node) Crash", false, true, false, reaction_karma::kick_player};
		reaction crash14{reaction_type::crash_player, reaction_sub_type::crash14, "Invalid sector position (sector node) Crash", false, true, false, reaction_karma::none};
		reaction crash15{reaction_type::crash_player, reaction_sub_type::crash15, "Invalid sector position (player game state node) Crash", false, true, false, reaction_karma::none};
		reaction crash16{reaction_type::crash_player, reaction_sub_type::crash16, "Out of bounds train track index Crash", false, true, false, reaction_karma::infraction_based};
		reaction crash17{reaction_type::crash_player, reaction_sub_type::crash17, "Out of bounds train config index Crash", false, true, false, reaction_karma::infraction_based};
		reaction crash18{reaction_type::crash_player, reaction_sub_type::crash18, "Out of bounds carriage config index Crash", false, true, false, reaction_karma::infraction_based};
		reaction crash19{reaction_type::crash_player, reaction_sub_type::crash19, "mount flag Crash", false, true, false, reaction_karma::kick_player};
		reaction crash20{reaction_type::crash_player, reaction_sub_type::crash20, "Invalid gadget Crash", false, true, false, reaction_karma::infraction_based};
		reaction crash21{reaction_type::crash_player, reaction_sub_type::crash21, "Submarine car (sync) Crash", false, true, false, reaction_karma::kick_player};
		reaction crash22{reaction_type::crash_player, reaction_sub_type::crash22, "Submarine car (creation) Crash", false, true, false, reaction_karma::kick_player};
		reaction crash23{reaction_type::crash_player, reaction_sub_type::crash23, "Invalid sector position (camera data node) Crash", false, true, false, reaction_karma::none};
		reaction crash24{reaction_type::crash_player, reaction_sub_type::crash24, "Out of bounds gadget type", false, true, false, reaction_karma::infraction_based};
		reaction crash25{reaction_type::crash_player, reaction_sub_type::crash25, "Invalid ped task Crash", false, true, false, reaction_karma::infraction_based};
		reaction crash26{reaction_type::crash_player, reaction_sub_type::crash26, "Invalid vehicle task Crash", false, true, false, reaction_karma::none};
		reaction crash27{reaction_type::crash_player, reaction_sub_type::crash27, "Invalid script info Crash", false, true, false, reaction_karma::infraction_based};
		reaction crash28{reaction_type::crash_player, reaction_sub_type::crash28, "Invalid interior Crash", false, true, false, reaction_karma::infraction_based};
		reaction crash29{reaction_type::crash_player, reaction_sub_type::crash29, "Out of bounds object type Crash", false, true, false, reaction_karma::infraction_based};
		reaction crash30{reaction_type::crash_player, reaction_sub_type::crash30, "Incorrect object type Crash", false, true, false, reaction_karma::infraction_based};
		reaction crash31{reaction_type::crash_player, reaction_sub_type::crash31, "Vehicle temp action Crash", false, true, false, reaction_karma::infraction_based};
		reaction crash32{reaction_type::crash_player, reaction_sub_type::crash32, "Invalid script entity change type Crash", false, true, false, reaction_karma::infraction_based};
		reaction crash33{reaction_type::crash_player, reaction_sub_type::crash33, "Rope Crash", false, true, false, reaction_karma::infraction_based};
		reaction crash34{reaction_type::crash_player, reaction_sub_type::crash34, "Pop group override Crash", false, true, false, reaction_karma::infraction_based};
		reaction crash35{reaction_type::crash_player, reaction_sub_type::crash35, "Invalid world state type Crash", false, true, false, reaction_karma::infraction_based};
		reaction crash36{reaction_type::crash_player, reaction_sub_type::crash36, "Remove unarmed Crash", false, true, false, reaction_karma::kick_player};
		reaction crash37{reaction_type::crash_player, reaction_sub_type::crash37, "Out of bounds give control type Crash", false, true, false, reaction_karma::kick_player};
		reaction crash38{reaction_type::crash_player, reaction_sub_type::crash38, "Invalid parachute object type Crash", false, true, false, reaction_karma::infraction_based};
		reaction crash39{reaction_type::crash_player, reaction_sub_type::crash39, "Invalid parachute model Crash", false, true, false, reaction_karma::kick_player};
		reaction crash40{reaction_type::crash_player, reaction_sub_type::crash40, "Out of bounds instance id Crash", false, true, false, reaction_karma::kick_player};
		reaction crash42{reaction_type::crash_player, reaction_sub_type::crash42, "Out of bounds tse args size Crash", false, true, false, reaction_karma::kick_player};
		reaction crash43{reaction_type::crash_player, reaction_sub_type::crash43, "Invalid vehicle parachute Crash", false, true, false, reaction_karma::kick_player};
		reaction player_ped_removal{reaction_type::crash_player, reaction_sub_type::player_ped_removal, "Player ped removal", false, true, false, reaction_karma::kick_player};
		reaction invalid_weapon_type{reaction_type::crash_player, reaction_sub_type::invalid_weapon_type, "invalid weapon type", false, true, false, reaction_karma::kick_player};
		reaction elegant_crash{reaction_type::crash_player, reaction_sub_type::elegant_crash, "Elegant Crash", false, true, false, reaction_karma::kick_player};

		// notify, not modder, other
		reaction kick_vote{reaction_type::none, reaction_sub_type::kick_vote, "Kick Vote", false, false, true};
		reaction report{reaction_type::none, reaction_sub_type::report, "Report", false, false, true};
		reaction bounty{reaction_type::none, reaction_sub_type::bounty, "Bounty", false, false, true};
		reaction request_control_event{reaction_type::none, reaction_sub_type::request_control_event, "Request Control Event", false, false, true};
		reaction vehicle_kick{reaction_type::none, reaction_sub_type::vehicle_kick, "Vehicle Kick", false, false, true};
		reaction send_to_interior{reaction_type::none, reaction_sub_type::send_to_interior, "Send To Interior", false, false, true};
		reaction nw_sound_spam_scanned{reaction_type::none, reaction_sub_type::nw_sound_spam_scanned, "Sound Spam (Netw. Scanned)", false, false, true};
		reaction sound_spam_invite_rate_limit{reaction_type::none, reaction_sub_type::sound_spam_invite_rate_limit, "Sound Spam (Invite Rate Limit)", false, false, true};
		reaction sound_spam_tse_rate_limit{reaction_type::none, reaction_sub_type::sound_spam_tse_rate_limit, "Sound Spam (TSE Rate Limit)", false, false, true};

		// notify once, not modder, other
		reaction rockstar_admin{reaction_type::none, reaction_sub_type::rockstar_admin, "Rockstar Admin", true, false, true};
		reaction nat_type_0_1{reaction_type::none, reaction_sub_type::nat_type_0_1, "Nat type 0/1", true, false, true};
		reaction spoofed_host_token_2{reaction_type::none, reaction_sub_type::spoofed_host_token_2, "Potentially Spoofed host token", true, false, true};

		// notify once, modder, not other
		reaction anti_cheat_modder_detection{reaction_type::none, reaction_sub_type::anti_cheat_modder_detection, "Detected modder by anti-cheat", true, true, false};
		reaction spoofed_host_token{reaction_type::none, reaction_sub_type::spoofed_host_token, "Spoofed host token", true, true, false};
		reaction spoofed_data{reaction_type::none, reaction_sub_type::spoofed_data, "Spoofed data", true, true, false};
		reaction invalid_player_model{reaction_type::none, reaction_sub_type::invalid_player_model, "Invalid player model", true, true, false};
		reaction super_jump{reaction_type::none, reaction_sub_type::super_jump, "Had used Super jump", true, true, false};
		reaction fake_deposit{reaction_type::none, reaction_sub_type::fake_deposit, "Fake Deposit", true, true, false};
		reaction rotate_cam{reaction_type::none, reaction_sub_type::rotate_cam, "Rotated your Camera", true, true, false};
		reaction transaction_error{reaction_type::none, reaction_sub_type::transaction_error, "Transaction Error", true, true, false};
		reaction remove_weapon{reaction_type::none, reaction_sub_type::remove_weapon, "Remove Weapon", true, true, false};
		reaction give_weapon{reaction_type::none, reaction_sub_type::give_weapon, "Give Weapon", true, true, false};
		reaction trigger_business_raid{reaction_type::none, reaction_sub_type::trigger_business_raid, "Trigger Business Raid", true, true, false};
		reaction turn_into_beast{reaction_type::none, reaction_sub_type::turn_into_beast, "Turn Into Beast", true, true, false};
		reaction report_cash_spawn{reaction_type::none, reaction_sub_type::report_cash_spawn, "Cash Spawn", true, true, false};
		reaction killed_ped_with_god{reaction_type::none, reaction_sub_type::killed_ped_with_god, "God Kill (Ped)", true, true, false};
		reaction killed_ped_with_invis{reaction_type::none, reaction_sub_type::killed_ped_with_invis, "Invisible Kill (Ped)", true, true, false};

		// notify, modder, not other
		reaction killed_with_god{reaction_type::none, reaction_sub_type::killed_with_god, "God Kill", false, true, false};
		reaction killed_with_invis{reaction_type::none, reaction_sub_type::killed_with_invis, "Invisible Kill", false, true, false};
		reaction killed_when_hidden{reaction_type::none, reaction_sub_type::killed_when_hidden, "Hidden Kill", false, true, false};
		reaction ceo_kick{reaction_type::none, reaction_sub_type::ceo_kick, "CEO Kick", false, true, false};
		reaction ceo_money{reaction_type::none, reaction_sub_type::ceo_money, "CEO Money", false, true, false};
		reaction clear_wanted_level{reaction_type::none, reaction_sub_type::clear_wanted_level, "Clear Wanted Level", false, true, false};
		reaction force_mission{reaction_type::none, reaction_sub_type::force_mission, "Force Mission", false, true, false};
		reaction force_teleport{reaction_type::none, reaction_sub_type::force_teleport, "Force Teleport", false, true, false};
		reaction kick_from_interior{reaction_type::none, reaction_sub_type::kick_from_interior, "Kick From Interior", false, true, false};
		reaction mc_teleport{reaction_type::none, reaction_sub_type::mc_teleport, "MC Teleport", false, true, false};
		reaction personal_vehicle_destroyed{reaction_type::none, reaction_sub_type::personal_vehicle_destroyed, "PV Destroyed", false, true, false};
		reaction destroy_personal_vehicle{reaction_type::none, reaction_sub_type::destroy_personal_vehicle, "Destroy PV", false, true, false};
		reaction give_collectible{reaction_type::none, reaction_sub_type::give_collectible, "Give Collectible", false, true, false};
		reaction remote_off_radar{reaction_type::none, reaction_sub_type::remote_off_radar, "Give Remote Off Radar", false, true, false};
		reaction send_to_cutscene{reaction_type::none, reaction_sub_type::send_to_cutscene, "Send To Cutscene", false, true, false};
		reaction send_to_location{reaction_type::none, reaction_sub_type::send_to_location, "Send To Location", false, true, false};
		reaction teleport_to_warehouse{reaction_type::none, reaction_sub_type::teleport_to_warehouse, "Teleport to warehouse", false, true, false};
		reaction start_activity{reaction_type::none, reaction_sub_type::start_activity, "Start Activity", false, true, false};
		reaction start_script{reaction_type::none, reaction_sub_type::start_script, "Start Script", false, true, false};
		reaction remote_wanted_level{reaction_type::none, reaction_sub_type::remote_wanted_level, "Remote Wanted Level", false, true, false};
		reaction clear_ped_tasks{reaction_type::none, reaction_sub_type::clear_ped_tasks, "Clear Ped Tasks", false, true, false};
		reaction veh_prox_mig_node{reaction_type::none, reaction_sub_type::veh_prox_mig_node, "CVehProxMigDataNode", false, true, false};
		reaction request_control_event_prob{reaction_type::none, reaction_sub_type::request_control_event_prob, "Request Control Event (Prob)", false, true, false};
		reaction vehicle_kick_prob{reaction_type::none, reaction_sub_type::vehicle_kick_prob, "Vehicle Kick (Prob)", false, true, false};

		// notify, not modder, not other
		reaction gta_banner{reaction_type::none, reaction_sub_type::gta_banner, "GTA Banner", false, false, false};
		reaction spectate{reaction_type::none, reaction_sub_type::spectate, "Spectate", false, false, false};
		reaction spectate_others{reaction_type::none, reaction_sub_type::spectate_others, "Spectate Others", false, false, false};
		reaction remote_ragdoll{reaction_type::none, reaction_sub_type::remote_ragdoll, "Remote Ragdoll", false, false, false, reaction_karma::none, 3};
		reaction spectate_notification{reaction_type::none, reaction_sub_type::spectate_notification, "Spectate Notification", false, false, false};
		reaction killed_with_veh_god{reaction_type::none, reaction_sub_type::killed_with_veh_god, "Vehicle God Kill", false, false, false};
		reaction killed_with_orbital{reaction_type::none, reaction_sub_type::killed_with_orbital, "Orbital Kill", false, false, false};
		reaction pop_multiplier_area{reaction_type::none, reaction_sub_type::pop_multiplier_area, "Pop Multiplier Area", false, false, false};
		reaction veh_spec_ability_event{reaction_type::none, reaction_sub_type::veh_spec_ability_event, "Vehicle Special Ability Event", false, false, false};
		reaction break_door_event{reaction_type::none, reaction_sub_type::break_door_event, "Break Door Event", false, false, false};
		reaction change_radio_station{reaction_type::none, reaction_sub_type::change_radio_station, "Change Radio Station", false, false, false};
		reaction tse_sender_mismatch{reaction_type::none, reaction_sub_type::tse_sender_mismatch, "TSE Sender Mismatch", false, false, false};
		reaction nw_sound_spam_rate_limit{reaction_type::none, reaction_sub_type::nw_sound_spam_rate_limit, "Sound Spam (Netw. Rate Limit)", false, false, false};
		reaction cage{reaction_type::none, reaction_sub_type::cage, "Cage", false, false, false};
		reaction ptfx_spam{reaction_type::none, reaction_sub_type::ptfx_spam, "Ptfx Spam", false, false, false};
	} g_reactions{};
}
