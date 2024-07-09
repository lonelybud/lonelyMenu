#pragma once
#include "backend/reactions/reaction.hpp"
#include "core/enums.hpp"

namespace big
{
	inline struct g_reactions_t
	{
		// kicks
		reaction end_session_kick{reaction_type::kick_player, reaction_sub_type::end_session_kick, "End Session Kick", reaction_notif_type::normal, player_type::modder, reaction_karma::kick_player};
		reaction network_bail{reaction_type::kick_player, reaction_sub_type::network_bail, "Network bail", reaction_notif_type::normal, player_type::modder, reaction_karma::kick_player};
		reaction null_function_kick{reaction_type::kick_player, reaction_sub_type::null_function_kick, "Null Function Kick", reaction_notif_type::normal, player_type::modder, reaction_karma::kick_player};
		reaction oom_kick{reaction_type::kick_player, reaction_sub_type::oom_kick, "OOM Kick (radio)", reaction_notif_type::normal, player_type::modder, reaction_karma::kick_player};
		reaction oom_kick2{reaction_type::kick_player, reaction_sub_type::oom_kick2, "OOM Kick (host migr.)", reaction_notif_type::normal, player_type::modder, reaction_karma::kick_player};
		reaction tse_freeze{reaction_type::kick_player, reaction_sub_type::tse_freeze, "TSE Freeze", reaction_notif_type::normal, player_type::modder, reaction_karma::kick_player};

		// crashes
		reaction scripted_event_crash{reaction_type::crash_player, reaction_sub_type::scripted_event_crash, "Scripted Event Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::kick_player};
		reaction stand_user_crash{reaction_type::crash_player, reaction_sub_type::stand_user_crash, "Stand User Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::kick_player};
		reaction crash1{reaction_type::crash_player, reaction_sub_type::crash1, "Invalid vehicle model Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::kick_player};
		reaction crash2{reaction_type::crash_player, reaction_sub_type::crash2, "Vehicle model mismatch Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::infraction_based};
		reaction crash3{reaction_type::crash_player, reaction_sub_type::crash3, "Invalid door model Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::kick_player};
		reaction crash4{reaction_type::crash_player, reaction_sub_type::crash4, "Invalid pickup model Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::infraction_based};
		reaction crash5{reaction_type::crash_player, reaction_sub_type::crash5, "Invalid pickup weapon component hash Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::infraction_based};
		reaction crash6{reaction_type::crash_player, reaction_sub_type::crash6, "Infinite physical attachment crash", reaction_notif_type::normal, player_type::modder, reaction_karma::kick_player};
		reaction crash7{reaction_type::crash_player, reaction_sub_type::crash7, "Invalid attachment Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::kick_player};
		reaction crash8{reaction_type::crash_player, reaction_sub_type::crash8, "Invalid ped model Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::none};
		reaction crash9{reaction_type::crash_player, reaction_sub_type::crash9, "Invalid ped prop model Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::infraction_based};
		reaction crash10{reaction_type::crash_player, reaction_sub_type::crash10, "Infinite ped attachment Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::infraction_based};
		reaction crash11{reaction_type::crash_player, reaction_sub_type::crash11, "Invalid object model Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::none};
		reaction crash12{reaction_type::crash_player, reaction_sub_type::crash12, "Invalid player model (appearance node) Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::kick_player};
		reaction crash13{reaction_type::crash_player, reaction_sub_type::crash13, "Invalid player model (creation node) Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::kick_player};
		reaction crash14{reaction_type::crash_player, reaction_sub_type::crash14, "Invalid sector position (sector node) Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::none};
		reaction crash15{reaction_type::crash_player, reaction_sub_type::crash15, "Invalid sector position (player game state node) Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::none};
		reaction crash16{reaction_type::crash_player, reaction_sub_type::crash16, "Out of bounds train track index Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::infraction_based};
		reaction crash17{reaction_type::crash_player, reaction_sub_type::crash17, "Out of bounds train config index Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::infraction_based};
		reaction crash18{reaction_type::crash_player, reaction_sub_type::crash18, "Out of bounds carriage config index Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::infraction_based};
		reaction crash19{reaction_type::crash_player, reaction_sub_type::crash19, "mount flag Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::kick_player};
		reaction crash20{reaction_type::crash_player, reaction_sub_type::crash20, "Invalid gadget Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::infraction_based};
		reaction crash21{reaction_type::crash_player, reaction_sub_type::crash21, "Submarine car (sync) Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::kick_player};
		reaction crash22{reaction_type::crash_player, reaction_sub_type::crash22, "Submarine car (creation) Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::kick_player};
		reaction crash23{reaction_type::crash_player, reaction_sub_type::crash23, "Invalid sector position (camera data node) Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::none};
		reaction crash24{reaction_type::crash_player, reaction_sub_type::crash24, "Out of bounds gadget type", reaction_notif_type::normal, player_type::modder, reaction_karma::infraction_based};
		reaction crash25{reaction_type::crash_player, reaction_sub_type::crash25, "Invalid ped task Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::infraction_based};
		reaction crash26{reaction_type::crash_player, reaction_sub_type::crash26, "Invalid vehicle task Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::none};
		reaction crash27{reaction_type::crash_player, reaction_sub_type::crash27, "Invalid script info Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::infraction_based};
		reaction crash28{reaction_type::crash_player, reaction_sub_type::crash28, "Invalid interior Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::infraction_based};
		reaction crash29{reaction_type::crash_player, reaction_sub_type::crash29, "Out of bounds object type Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::infraction_based};
		reaction crash30{reaction_type::crash_player, reaction_sub_type::crash30, "Incorrect object type Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::infraction_based};
		reaction crash31{reaction_type::crash_player, reaction_sub_type::crash31, "Vehicle temp action Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::infraction_based};
		reaction crash32{reaction_type::crash_player, reaction_sub_type::crash32, "Invalid script entity change type Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::infraction_based};
		reaction crash34{reaction_type::crash_player, reaction_sub_type::crash34, "Pop group override Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::infraction_based};
		reaction crash35{reaction_type::crash_player, reaction_sub_type::crash35, "Invalid world state type Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::infraction_based};
		reaction crash36{reaction_type::crash_player, reaction_sub_type::crash36, "Remove unarmed Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::kick_player};
		reaction crash37{reaction_type::crash_player, reaction_sub_type::crash37, "Out of bounds give control type Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::kick_player};
		reaction crash38{reaction_type::crash_player, reaction_sub_type::crash38, "Invalid parachute object type Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::infraction_based};
		reaction crash39{reaction_type::crash_player, reaction_sub_type::crash39, "Invalid parachute model Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::kick_player};
		reaction crash40{reaction_type::crash_player, reaction_sub_type::crash40, "Out of bounds instance id Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::kick_player};
		reaction crash43{reaction_type::crash_player, reaction_sub_type::crash43, "Invalid vehicle parachute Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::kick_player};
		reaction player_ped_remove{reaction_type::crash_player, reaction_sub_type::player_ped_remove, "Player ped removal", reaction_notif_type::normal, player_type::modder, reaction_karma::kick_player};
		reaction invalid_weapon_type{reaction_type::crash_player, reaction_sub_type::invalid_weapon_type, "invalid weapon type", reaction_notif_type::normal, player_type::modder, reaction_karma::kick_player};
		reaction elegant_crash{reaction_type::crash_player, reaction_sub_type::elegant_crash, "Elegant Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::kick_player};
		reaction player_ped_create{reaction_type::crash_player, reaction_sub_type::player_ped_create, "Player Create", reaction_notif_type::normal, player_type::modder, reaction_karma::kick_player};
		reaction player_ped_sync{reaction_type::crash_player, reaction_sub_type::player_ped_sync, "Player Sync", reaction_notif_type::normal, player_type::modder, reaction_karma::kick_player};

		reaction crash33{reaction_type::crash_player, reaction_sub_type::crash33, "Rope Crash", reaction_notif_type::normal, player_type::modder, reaction_karma::none};

		// normal, other
		reaction kick_vote{reaction_type::none, reaction_sub_type::kick_vote, "Kick Vote", reaction_notif_type::normal, player_type::other};
		reaction report{reaction_type::none, reaction_sub_type::report, "Report", reaction_notif_type::normal, player_type::other};
		reaction bounty{reaction_type::none, reaction_sub_type::bounty, "Bounty", reaction_notif_type::normal, player_type::other};
		reaction request_control_event{reaction_type::none, reaction_sub_type::request_control_event, "Request Control Event", reaction_notif_type::normal, player_type::other};
		reaction vehicle_kick{reaction_type::none, reaction_sub_type::vehicle_kick, "Vehicle Kick", reaction_notif_type::normal, player_type::other};
		reaction send_to_interior{reaction_type::none, reaction_sub_type::send_to_interior, "Send To Interior", reaction_notif_type::normal, player_type::other};
		reaction nw_sound_spam_scanned{reaction_type::none, reaction_sub_type::nw_sound_spam_scanned, "Sound Spam (Netw. Scanned)", reaction_notif_type::normal, player_type::other};
		reaction sound_spam_invite_rate_limit{reaction_type::none, reaction_sub_type::sound_spam_invite_rate_limit, "Sound Spam (Invite Rate Limit)", reaction_notif_type::normal, player_type::other};
		reaction sound_spam_tse_rate_limit{reaction_type::none, reaction_sub_type::sound_spam_tse_rate_limit, "Sound Spam (TSE Rate Limit)", reaction_notif_type::normal, player_type::other};
		reaction blamed_explosion_detected{reaction_type::none, reaction_sub_type::blame_explosion_detected, "Blame Explosion", reaction_notif_type::normal, player_type::other};

		// once, other
		reaction rockstar_admin{reaction_type::none, reaction_sub_type::rockstar_admin, "Rockstar Admin", reaction_notif_type::once, player_type::other};
		reaction spoofed_host_token_2{reaction_type::none, reaction_sub_type::spoofed_host_token_2, "Potentially Spoofed host token", reaction_notif_type::once, player_type::other};

		// silent, other
		reaction nat_type_0_1{reaction_type::none, reaction_sub_type::nat_type_0_1, "Nat type 0/1", reaction_notif_type::silent, player_type::other};

		// once, modder
		reaction anti_cheat_modder_detection{reaction_type::none, reaction_sub_type::anti_cheat_modder_detection, "Detected modder by anti-cheat", reaction_notif_type::once, player_type::modder};
		reaction spoofed_host_token{reaction_type::none, reaction_sub_type::spoofed_host_token, "Spoofed host token", reaction_notif_type::once, player_type::modder};
		reaction spoofed_data{reaction_type::none, reaction_sub_type::spoofed_data, "Spoofed data", reaction_notif_type::once, player_type::modder};
		reaction invalid_player_model{reaction_type::none, reaction_sub_type::invalid_player_model, "Invalid player model", reaction_notif_type::once, player_type::modder};
		reaction super_jump{reaction_type::none, reaction_sub_type::super_jump, "Had used Super jump", reaction_notif_type::once, player_type::modder};
		reaction remove_weapon{reaction_type::none, reaction_sub_type::remove_weapon, "Remove Weapon", reaction_notif_type::once, player_type::modder};
		reaction give_weapon{reaction_type::none, reaction_sub_type::give_weapon, "Give Weapon", reaction_notif_type::once, player_type::modder};
		reaction trigger_business_raid{reaction_type::none, reaction_sub_type::trigger_business_raid, "Trigger Business Raid", reaction_notif_type::once, player_type::modder};
		reaction turn_into_beast{reaction_type::none, reaction_sub_type::turn_into_beast, "Turn Into Beast", reaction_notif_type::once, player_type::modder};
		reaction report_cash_spawn{reaction_type::none, reaction_sub_type::report_cash_spawn, "Cash Spawn", reaction_notif_type::once, player_type::modder};
		reaction killed_ped_with_god{reaction_type::none, reaction_sub_type::killed_ped_with_god, "God Kill (Ped)", reaction_notif_type::once, player_type::modder};
		reaction killed_ped_with_invis{reaction_type::none, reaction_sub_type::killed_ped_with_invis, "Invisible Kill (Ped)", reaction_notif_type::once, player_type::modder};
		reaction sent_modder_beacons{reaction_type::none, reaction_sub_type::sent_modder_beacons, "Sent Modder Beacons", reaction_notif_type::once, player_type::modder};

		reaction fake_deposit{reaction_type::none, reaction_sub_type::fake_deposit, "Fake Deposit", reaction_notif_type::once, player_type::modder};
		reaction rotate_cam{reaction_type::none, reaction_sub_type::rotate_cam, "Rotated your Camera", reaction_notif_type::once, player_type::modder};
		reaction transaction_error{reaction_type::none, reaction_sub_type::transaction_error, "Transaction Error", reaction_notif_type::once, player_type::modder};
		reaction clear_wanted_level{reaction_type::none, reaction_sub_type::clear_wanted_level, "Clear Wanted Level", reaction_notif_type::once, player_type::modder};
		reaction ceo_money{reaction_type::none, reaction_sub_type::ceo_money, "CEO Money", reaction_notif_type::once, player_type::modder};
		reaction remote_off_radar{reaction_type::none, reaction_sub_type::remote_off_radar, "Give Remote Off Radar", reaction_notif_type::once, player_type::modder};
		reaction give_collectible{reaction_type::none, reaction_sub_type::give_collectible, "Give Collectible", reaction_notif_type::once, player_type::modder};
		reaction remote_wanted_level{reaction_type::none, reaction_sub_type::remote_wanted_level, "Remote Wanted Level", reaction_notif_type::once, player_type::modder};

		// normal, modder
		reaction killed_with_god{reaction_type::none, reaction_sub_type::killed_with_god, "God Kill", reaction_notif_type::normal, player_type::modder};
		reaction killed_with_invis{reaction_type::none, reaction_sub_type::killed_with_invis, "Invisible Kill", reaction_notif_type::normal, player_type::modder};
		reaction killed_when_hidden{reaction_type::none, reaction_sub_type::killed_when_hidden, "Hidden Kill", reaction_notif_type::normal, player_type::modder};
		reaction ceo_kick{reaction_type::none, reaction_sub_type::ceo_kick, "CEO Kick", reaction_notif_type::normal, player_type::modder};
		reaction force_mission{reaction_type::none, reaction_sub_type::force_mission, "Force Mission", reaction_notif_type::normal, player_type::modder};
		reaction force_teleport{reaction_type::none, reaction_sub_type::force_teleport, "Force Teleport", reaction_notif_type::normal, player_type::modder};
		reaction kick_from_interior{reaction_type::none, reaction_sub_type::kick_from_interior, "Kick From Interior", reaction_notif_type::normal, player_type::modder};
		reaction mc_teleport{reaction_type::none, reaction_sub_type::mc_teleport, "MC Teleport", reaction_notif_type::normal, player_type::modder};
		reaction personal_vehicle_destroyed{reaction_type::none, reaction_sub_type::personal_vehicle_destroyed, "PV Destroyed", reaction_notif_type::normal, player_type::modder};
		reaction destroy_personal_vehicle{reaction_type::none, reaction_sub_type::destroy_personal_vehicle, "Destroy PV", reaction_notif_type::normal, player_type::modder};
		reaction send_to_cutscene{reaction_type::none, reaction_sub_type::send_to_cutscene, "Send To Cutscene", reaction_notif_type::normal, player_type::modder};
		reaction send_to_location{reaction_type::none, reaction_sub_type::send_to_location, "Send To Location", reaction_notif_type::normal, player_type::modder};
		reaction teleport_to_warehouse{reaction_type::none, reaction_sub_type::teleport_to_warehouse, "Teleport to warehouse", reaction_notif_type::normal, player_type::modder};
		reaction start_activity{reaction_type::none, reaction_sub_type::start_activity, "Start Activity", reaction_notif_type::normal, player_type::modder};
		reaction start_script_begin{reaction_type::none, reaction_sub_type::start_script_begin, "Start Script Begin", reaction_notif_type::normal, player_type::modder};
		reaction start_script_proceed{reaction_type::none, reaction_sub_type::start_script_proceed, "Start Script Proceed", reaction_notif_type::normal, player_type::modder};
		reaction clear_ped_tasks{reaction_type::none, reaction_sub_type::clear_ped_tasks, "Clear Ped Tasks", reaction_notif_type::normal, player_type::modder};
		reaction veh_prox_mig_node{reaction_type::none, reaction_sub_type::veh_prox_mig_node, "CVehProxMigDataNode", reaction_notif_type::normal, player_type::modder};
		reaction request_control_event_prob{reaction_type::none, reaction_sub_type::request_control_event_prob, "Request Control Event (Prob)", reaction_notif_type::normal, player_type::modder};
		reaction vehicle_kick_prob{reaction_type::none, reaction_sub_type::vehicle_kick_prob, "Vehicle Kick (Prob)", reaction_notif_type::normal, player_type::modder};
		reaction break_game{reaction_type::none, reaction_sub_type::break_game, "Break Game", reaction_notif_type::normal, player_type::modder};

		// normal, normal
		reaction gta_banner{reaction_type::none, reaction_sub_type::gta_banner, "GTA Banner", reaction_notif_type::normal, player_type::normal};
		reaction spectate{reaction_type::none, reaction_sub_type::spectate, "Spectate", reaction_notif_type::normal, player_type::normal};
		reaction spectate_others{reaction_type::none, reaction_sub_type::spectate_others, "Spectate Others", reaction_notif_type::normal, player_type::normal};
		reaction remote_ragdoll{reaction_type::none, reaction_sub_type::remote_ragdoll, "Remote Ragdoll", reaction_notif_type::normal, player_type::normal};
		reaction spectate_notification{reaction_type::none, reaction_sub_type::spectate_notification, "Spectate Notification", reaction_notif_type::normal, player_type::normal};
		reaction killed_with_veh_god{reaction_type::none, reaction_sub_type::killed_with_veh_god, "Vehicle God Kill", reaction_notif_type::normal, player_type::normal};
		reaction killed_with_orbital{reaction_type::none, reaction_sub_type::killed_with_orbital, "Orbital Kill", reaction_notif_type::normal, player_type::normal};
		reaction pop_multiplier_area{reaction_type::none, reaction_sub_type::pop_multiplier_area, "Pop Multiplier Area", reaction_notif_type::normal, player_type::normal};
		reaction veh_spec_ability_event{reaction_type::none, reaction_sub_type::veh_spec_ability_event, "Vehicle Special Ability Event", reaction_notif_type::normal, player_type::normal};
		reaction break_door_event{reaction_type::none, reaction_sub_type::break_door_event, "Break Door Event", reaction_notif_type::normal, player_type::normal};
		reaction change_radio_station{reaction_type::none, reaction_sub_type::change_radio_station, "Change Radio Station", reaction_notif_type::normal, player_type::normal};
		reaction tse_sender_mismatch{reaction_type::none, reaction_sub_type::tse_sender_mismatch, "TSE Sender Mismatch", reaction_notif_type::normal, player_type::normal};
		reaction nw_sound_spam_rate_limit{reaction_type::none, reaction_sub_type::nw_sound_spam_rate_limit, "Sound Spam (Netw. Rate Limit)", reaction_notif_type::normal, player_type::normal};
		reaction cage{reaction_type::none, reaction_sub_type::cage, "Cage", reaction_notif_type::normal, player_type::normal};
		reaction ptfx_spam{reaction_type::none, reaction_sub_type::ptfx_spam, "Ptfx Spam", reaction_notif_type::normal, player_type::normal};
		reaction delete_vehicle_cc{reaction_type::none, reaction_sub_type::delete_vehicle_cc, "Delete Vehicle (CC)", reaction_notif_type::normal, player_type::normal};
		reaction delete_vehicle_cr{reaction_type::none, reaction_sub_type::delete_vehicle_cr, "Delete Vehicle (CR)", reaction_notif_type::normal, player_type::normal};

		// used in view::reaction_settings. It is dummy and useless
		reaction _none{reaction_type::none, reaction_sub_type::none, "", reaction_notif_type::normal, player_type::normal};
	} g_reactions{};
}
