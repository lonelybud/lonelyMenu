#pragma once
#include "backend/reactions/reaction.hpp"
#include "core/enums.hpp"

namespace big
{
	inline struct g_reactions_t
	{
		reaction end_session_kick{reaction_type::kick_player, reaction_sub_type::end_session_kick, "End Session Kick", "End Session Kick", false, true, false};
		reaction network_bail{reaction_type::kick_player, reaction_sub_type::network_bail, "Network bail", "Network bail", false, true, false};
		reaction null_function_kick{reaction_type::kick_player, reaction_sub_type::null_function_kick, "Null Function Kick", "Null Function Kick", false, true, false};
		reaction oom_kick{reaction_type::kick_player, reaction_sub_type::oom_kick, "OOM Kick (radio)", "OOM Kick (radio)", false, true, false};
		reaction oom_kick2{reaction_type::kick_player, reaction_sub_type::oom_kick2, "OOM Kick (host migr.)", "OOM Kick (host migr.)", false, true, false};
		reaction tse_freeze{reaction_type::kick_player, reaction_sub_type::tse_freeze, "TSE Freeze", "TSE Freeze", false, true, false};
		reaction tse_sender_mismatch{reaction_type::kick_player, reaction_sub_type::tse_sender_mismatch, "TSE Sender Mismatch", "TSE Sender Mismatch", false, true, false};

		reaction scripted_event_crash{reaction_type::crash_player, reaction_sub_type::scripted_event_crash, "Scripted Event Crash", "Scripted Event Crash", false, true, false};
		reaction stand_user_crash{reaction_type::crash_player, reaction_sub_type::stand_user_crash, "Stand User Crash", "Stand User Crash", false, true, false};
		reaction crash1{reaction_type::crash_player, reaction_sub_type::crash1, "Crash 1", "invalid vehicle model", false, true, false};
		reaction crash2{reaction_type::crash_player, reaction_sub_type::crash2, "Crash 2", "vehicle model mismatch", false, true, false};
		reaction crash3{reaction_type::crash_player, reaction_sub_type::crash3, "Crash 3", "invalid door model", false, true, false};
		reaction crash4{reaction_type::crash_player, reaction_sub_type::crash4, "Crash 4", "invalid pickup model", false, true, false};
		reaction crash5{reaction_type::crash_player, reaction_sub_type::crash5, "Crash 5", "invalid pickup weapon component hash", false, true, false};
		reaction crash6{reaction_type::crash_player, reaction_sub_type::crash6, "Crash 6", "infinite physical attachment", false, true, false};
		reaction crash7{reaction_type::crash_player, reaction_sub_type::crash7, "Crash 7", "invalid attachment", false, true, false};
		reaction crash8{reaction_type::crash_player, reaction_sub_type::crash8, "Crash 8", "invalid ped model", false, true, false};
		reaction crash9{reaction_type::crash_player, reaction_sub_type::crash9, "Crash 9", "invalid ped prop model", false, true, false};
		reaction crash10{reaction_type::crash_player, reaction_sub_type::crash10, "Crash 10", "infinite ped attachment", false, true, false};
		reaction crash11{reaction_type::crash_player, reaction_sub_type::crash11, "Crash 11", "invalid object model", false, true, false};
		reaction crash12{reaction_type::crash_player, reaction_sub_type::crash12, "Crash 12", "invalid player model (appearance node)", false, true, false};
		reaction crash13{reaction_type::crash_player, reaction_sub_type::crash13, "Crash 13", "invalid player model (creation node)", false, true, false};
		reaction crash14{reaction_type::crash_player, reaction_sub_type::crash14, "Crash 14", "invalid sector position (sector node)", false, true, false};
		reaction crash15{reaction_type::crash_player, reaction_sub_type::crash15, "Crash 15", "invalid sector position (player game state node)", false, true, false};
		reaction crash16{reaction_type::crash_player, reaction_sub_type::crash16, "Crash 16", "out of bounds train track index", false, true, false};
		reaction crash17{reaction_type::crash_player, reaction_sub_type::crash17, "Crash 17", "out of bounds train config index", false, true, false};
		reaction crash18{reaction_type::crash_player, reaction_sub_type::crash18, "Crash 18", "out of bounds carriage config index", false, true, false};
		reaction crash19{reaction_type::crash_player, reaction_sub_type::crash19, "Crash 19", "mount flag", false, true, false};
		reaction crash20{reaction_type::crash_player, reaction_sub_type::crash20, "Crash 20", "invalid gadget", false, true, false};
		reaction crash21{reaction_type::crash_player, reaction_sub_type::crash21, "Crash 21", "submarine car (sync)", false, true, false};
		reaction crash22{reaction_type::crash_player, reaction_sub_type::crash22, "Crash 22", "submarine car (creation)", false, true, false};
		reaction crash23{reaction_type::crash_player, reaction_sub_type::crash23, "Crash 23", "invalid sector position (camera data node)", false, true, false};
		reaction crash24{reaction_type::crash_player, reaction_sub_type::crash24, "Crash 24", "out of bounds gadget type", false, true, false};
		reaction crash25{reaction_type::crash_player, reaction_sub_type::crash25, "Crash 25", "invalid ped task", false, true, false};
		reaction crash26{reaction_type::crash_player, reaction_sub_type::crash26, "Crash 26", "invalid vehicle task", false, true, false};
		reaction crash27{reaction_type::crash_player, reaction_sub_type::crash27, "Crash 27", "invalid script info", false, true, false};
		reaction crash28{reaction_type::crash_player, reaction_sub_type::crash28, "Crash 28", "invalid interior", false, true, false};
		reaction crash29{reaction_type::crash_player, reaction_sub_type::crash29, "Crash 29", "out of bounds object type", false, true, false};
		reaction crash30{reaction_type::crash_player, reaction_sub_type::crash30, "Crash 30", "incorrect object type", false, true, false};
		reaction crash31{reaction_type::crash_player, reaction_sub_type::crash31, "Crash 31", "vehicle temp action", false, true, false};
		reaction crash32{reaction_type::crash_player, reaction_sub_type::crash32, "Crash 32", "invalid script entity change type", false, true, false};
		reaction crash33{reaction_type::crash_player, reaction_sub_type::crash33, "Crash 33", "rope", false, true, false};
		reaction crash34{reaction_type::crash_player, reaction_sub_type::crash34, "Crash 34", "pop group override", false, true, false};
		reaction crash35{reaction_type::crash_player, reaction_sub_type::crash35, "Crash 35", "invalid world state type", false, true, false};
		reaction crash36{reaction_type::crash_player, reaction_sub_type::crash36, "Crash 36", "remove unarmed", false, true, false};
		reaction crash37{reaction_type::crash_player, reaction_sub_type::crash37, "Crash 37", "out of bounds give control type", false, true, false};
		reaction crash38{reaction_type::crash_player, reaction_sub_type::crash38, "Crash 38", "invalid parachute object type", false, true, false};
		reaction crash39{reaction_type::crash_player, reaction_sub_type::crash39, "Crash 39", "invalid parachute model", false, true, false};
		reaction crash40{reaction_type::crash_player, reaction_sub_type::crash40, "Crash 40", "out of bounds instance id", false, true, false};
		reaction player_ped_removal{reaction_type::crash_player, reaction_sub_type::player_ped_removal, "Player ped removal", "Player ped removal", false, true, false};
		reaction invalid_weapon_type{reaction_type::crash_player, reaction_sub_type::invalid_weapon_type, "invalid weapon type", "invalid weapon type", false, true, false};

		reaction kick_vote{reaction_type::none, reaction_sub_type::kick_vote, "Kick Vote", "Voting to kick you", true, false, true};
		reaction report{reaction_type::none, reaction_sub_type::report, "Report", "Reported you", true, false, true};
		reaction bounty{reaction_type::none, reaction_sub_type::bounty, "Bounty", "Set bounty on you", true, false, true};

		reaction anti_cheat_modder_detection{reaction_type::modder_detection, reaction_sub_type::anti_cheat_modder_detection, "Anti cheat modder detection", "Detected modder by anti-cheat", true, true, false};
		reaction spoofed_host_token{reaction_type::modder_detection, reaction_sub_type::spoofed_host_token, "Spoofed host token", "Spoofed their host token", true, true, false};
		reaction killed_with_god{reaction_type::modder_detection, reaction_sub_type::killed_with_god, "Killed with god", "Killed someone with god", false, true, false};
		reaction killed_with_veh_god{reaction_type::modder_detection, reaction_sub_type::killed_with_veh_god, "Killed with vehicle god", "Killed someone with vehicle god", false, false, false};
		reaction killed_with_invis{reaction_type::modder_detection, reaction_sub_type::killed_with_invis, "Killed when invisible", "Killed someone when invisible", false, true, false};
		reaction killed_when_hidden{reaction_type::modder_detection, reaction_sub_type::killed_when_hidden, "Killed when hidden", "Killed someone while hidden in list", false, true, false};
		reaction Killed_with_orbital{reaction_type::modder_detection, reaction_sub_type::Killed_with_orbital, "Killed with orbital", "Killed someone with orbital canon", false, true, false};
		reaction spoofed_data{reaction_type::modder_detection, reaction_sub_type::spoofed_data, "Spoofed data", "Had spoofed their data", true, true, false};
		reaction invalid_player_model{reaction_type::modder_detection, reaction_sub_type::invalid_player_model, "Invalid player model", "Had used an invalid player model", true, true, false};
		reaction super_jump{reaction_type::modder_detection, reaction_sub_type::super_jump, "Super jump", "Had used super jump", true, true, false};

		reaction gta_banner{reaction_type::none, reaction_sub_type::gta_banner, "GTA Banner", "Received GTA Banner", false, false, false, 1};
		reaction sound_spam{reaction_type::none, reaction_sub_type::sound_spam, "Sound Spam", "Received Sound Spam", false, false, false};
		reaction spectate{reaction_type::none, reaction_sub_type::spectate, "Spectate", "Received Spectate", false, false, false};
		reaction remote_ragdoll{reaction_type::none, reaction_sub_type::remote_ragdoll, "Remote Ragdoll", "Received Remote Ragdoll", false, false, false, 1};
		reaction spectate_notification{reaction_type::none, reaction_sub_type::spectate_notification, "Spectate Notification", "Received Spectate Notification", false, false, false};

		reaction ceo_kick{reaction_type::none, reaction_sub_type::ceo_kick, "CEO Kick", "CEO Kick", false, true, false, 2};
		reaction ceo_money{reaction_type::none, reaction_sub_type::ceo_money, "CEO Money", "CEO Money", false, true, false, 2};
		reaction clear_wanted_level{reaction_type::none, reaction_sub_type::clear_wanted_level, "Clear Wanted Level", "Clear Wanted Level", false, true, false, 2};
		reaction fake_deposit{reaction_type::none, reaction_sub_type::fake_deposit, "Fake Deposit", "Fake Deposit", true, true, false};
		reaction force_mission{reaction_type::none, reaction_sub_type::force_mission, "Force Mission", "Force Mission", false, true, false, 1};
		reaction force_teleport{reaction_type::none, reaction_sub_type::force_teleport, "Force Teleport", "Force Teleport", false, true, false, 1};
		reaction kick_from_interior{reaction_type::none, reaction_sub_type::kick_from_interior, "Kick From Interior", "Kick From Interior", false, true, false, 1};
		reaction mc_teleport{reaction_type::none, reaction_sub_type::mc_teleport, "MC Teleport", "MC Teleport", false, true, false, 1};
		reaction personal_vehicle_destroyed{reaction_type::none, reaction_sub_type::personal_vehicle_destroyed, "Per. Veh Destroyed", "Personal Vehicle Destroyed", false, true, false, 1};
		reaction destroy_personal_vehicle{reaction_type::none, reaction_sub_type::destroy_personal_vehicle, "Destroy Per. Veh", "Destroy Personal Vehicle", false, true, false, 1};
		reaction give_collectible{reaction_type::none, reaction_sub_type::give_collectible, "Give Collectible", "Given you Collectible", false, true, false, 1};
		reaction remote_off_radar{reaction_type::none, reaction_sub_type::remote_off_radar, "Remote Off Radar", "Given you Remote Off Radar", false, true, false, 1};
		reaction rotate_cam{reaction_type::none, reaction_sub_type::rotate_cam, "Rotate Cam", "Rotated your Camera", true, true, false};
		reaction send_to_cutscene{reaction_type::none, reaction_sub_type::send_to_cutscene, "Send To Cutscene", "Received Send To Cutscene", true, true, false};
		reaction send_to_location{reaction_type::none, reaction_sub_type::send_to_location, "Send To Location", "Received Send To Location", true, true, false};
		reaction send_to_interior{reaction_type::none, reaction_sub_type::send_to_interior, "Send To Interior", "Received Send To Interior", true, true, false};
		reaction transaction_error{reaction_type::none, reaction_sub_type::transaction_error, "Transaction Error", "Received Transaction Error", true, true, false};
		reaction vehicle_kick{reaction_type::none, reaction_sub_type::vehicle_kick, "Vehicle Kick", "Received Vehicle Kick", false, true, false, 2};
		reaction teleport_to_warehouse{reaction_type::none, reaction_sub_type::teleport_to_warehouse, "Teleport to warehouse", "Received Teleport to warehouse", true, true, false};
		reaction start_activity{reaction_type::none, reaction_sub_type::start_activity, "Start Activity", "Received Start Activity", false, true, false, 1};
		reaction start_script{reaction_type::none, reaction_sub_type::start_script, "Start Script", "Received Start Script", true, true, false};
		reaction trigger_business_raid{reaction_type::none, reaction_sub_type::trigger_business_raid, "Trigger Business Raid", "Tried to Trigger Business Raid", true, true, false};
		reaction turn_into_beast{reaction_type::none, reaction_sub_type::turn_into_beast, "Turn Into Beast", "Received Turn Into Beast", true, true, false};
		reaction remote_wanted_level{reaction_type::none, reaction_sub_type::remote_wanted_level, "Remote Wanted Level", "Received Remote Wanted Level", true, true, false};
		reaction clear_ped_tasks{reaction_type::none, reaction_sub_type::clear_ped_tasks, "Clear Ped Tasks", "Received Clear Ped Tasks", false, true, false, 2};
		reaction report_cash_spawn{reaction_type::none, reaction_sub_type::report_cash_spawn, "Cash Spawn", "Received Cash Spawn", true, true, false};
		reaction request_control_event{reaction_type::none, reaction_sub_type::request_control_event, "Request Control Event", "Received Request Control Event", false, true, false, 2};
		reaction remove_weapon{reaction_type::none, reaction_sub_type::remove_weapon, "Remove Weapon", "Received Remove Weapon", true, true, false};
		reaction give_weapon{reaction_type::none, reaction_sub_type::give_weapon, "Give Weapon", "Received Give Weapon", true, true, false};
	} g_reactions{};
}
