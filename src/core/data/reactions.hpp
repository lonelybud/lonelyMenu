#pragma once
#include "backend/reactions/reaction.hpp"

namespace big
{
	inline struct g_reactions_t
	{
		reaction end_session_kick{1, "End Session Kick", "Blocked End Session Kick from {}"};
		reaction network_bail{2, "Network Bail", "Blocked Network Bail from {}"};
		reaction null_function_kick{3, "Null Function Kick", "Blocked Null Function Kick from {}"};
		reaction kick_vote{4, "Kick Vote", "{} is voting to kick you!"};
		reaction oom_kick{5, "OOM Kick", "Blocked OOM Kick (from radio requests) from {}"};
		reaction oom_kick2{6, "OOM Kick", "Blocked OOM Kick (from host migr. requests) from {}"};
		reaction crash{7, "Crash", "Blocked Crash from {}"};
		reaction stand_user_crash{8, "Crash", "Blocked Stand User Crash from {}"};
		reaction report{9, "Report", "Blocked Report from {}"};

		reaction gta_banner{10, "GTA Banner", "Blocked GTA Banner from {}"}; // please don't enable this
		reaction tse_freeze{11, "TSE Freeze", "Blocked TSE Freeze from {}"};
		reaction tse_sender_mismatch{12, "TSE Sender Mismatch", "Blocked TSE Sender Mismatch from {}"};

		reaction game_anti_cheat_modder_detection{13, "Anti-Cheat Modder Detection", "{} is detected as a modder by anti-cheat!"};
		reaction modder_detection{14, "Modder Detection", "{} is detected as a modder!"};

		reaction bounty{15, "Bounty", "Received Bounty from {}"};
		reaction ceo_kick{16, "CEO Kick", "Blocked CEO Kick from {}"};
		reaction ceo_money{17, "CEO Money", "Blocked CEO Money from {}"};
		reaction clear_wanted_level{18, "Clear Wanted Level", "Blocked Clear Wanted Level from {}"};
		reaction fake_deposit{19, "Fake Deposit", "Blocked Fake Deposit from {}"};
		reaction force_mission{20, "Force Mission", "Blocked Force Mission from {}"};
		reaction force_teleport{21, "Force Teleport", "Blocked Force Teleport from {}"};
		reaction kick_from_interior{22, "Kick From Interior", "Blocked Kick From Interior from {}"};
		reaction mc_teleport{23, "MC Teleport", "Blocked MC Teleport from {}"};
		reaction personal_vehicle_destroyed{24, "Personal Vehicle Destroyed", "Blocked Personal Vehicle Destroyed from {}"};
		reaction destroy_personal_vehicle{25, "Destroy Personal Vehicle", "Blocked Destroy Personal Vehicle from {}"};
		reaction give_collectible{26, "Give Collectible", "Blocked Give Collectible from {}"};
		reaction remote_off_radar{27, "Remote Off Radar", "Blocked Remote Off Radar from {}"};
		reaction rotate_cam{28, "Rotate Cam", "Blocked Rotate Cam from {}"};
		reaction send_to_cutscene{29, "Send To Cutscene", "Blocked Send To Cutscene from {}"};
		reaction send_to_location{30, "Send To Location", "Blocked Send To Location from {}"};
		reaction send_to_interior{31, "Send To Interior", "Blocked Send To Interior from {}"};
		reaction sound_spam{32, "Sound Spam", "Blocked Sound Spam from {}"};
		reaction spectate_notification{33, "Spectate Notification", "Blocked Spectate Notification from {}"};
		reaction transaction_error{34, "Transaction Error", "Blocked Transaction Error from {}"};
		reaction vehicle_kick{35, "Vehicle Kick", "Blocked Vehicle Kick from {}"};
		reaction teleport_to_warehouse{36, "Teleport To Warehouse", "Blocked Teleport To Warehouse from {}"};
		reaction start_activity{37, "Start Activity", "Blocked Start Activity from {}"};
		reaction start_script{38, "Start Script", "Blocked Start Script from {}"};
		reaction trigger_business_raid{39, "Trigger Business Raid", "Blocked Trigger Business Raid from {}"};
		reaction turn_into_beast{40, "Turn Into Beast", "Blocked Turn Into Beast from {}"};
		reaction remote_wanted_level{41, "Remote Wanted Level", "Blocked Remote Wanted Level from {}"};
		reaction clear_ped_tasks{42, "Clear Ped Tasks", "Blocked Clear Ped Tasks from {}"};
		reaction remote_ragdoll{43, "Remote Ragdoll", "Blocked Remote Ragdoll from {}"};
		reaction report_cash_spawn{44, "Cash Spawn", "{} is spawning cash!"};
		reaction request_control_event{45, "Request Control Event", "Blocked Request Control Event from {}"};
		reaction spectate{46, "Spectate", "{} is spectating you"};

		reaction remove_weapon{47, "Remove Weapon", "Blocked remove weapon from {}"};
		reaction give_weapon{48, "Give Weapon", "Blocked give weapon from {}"};
		reaction spoofed_host_token{49, "Spoofed Host token", "{} has spoofed their host token."};

		reaction cheater_joined{14, "Cheater Joined", "{} is a cheater marked by rockstar!"};
		reaction rockstar_dev{14, "Rockstar player", "{} is detected as DEV or QA!"};
	} g_reactions{};

	inline std::map<int, const char*> crashes = {
	    {1, "invalid vehicle model"},
	    {2, "vehicle model mismatch"},
	    {3, "invalid door model"},
	    {4, "invalid pickup model"},
	    {5, "invalid pickup weapon component hash"},
	    {6, "infinite physical attachment"},
	    {7, "invalid attachment"},
	    {8, "invalid ped model"},
	    {9, "invalid ped prop model"},
	    {10, "infinite ped attachment"},
	    {11, "invalid object model"},
	    {12, "invalid player model (appearance node)"},
	    {13, "invalid player model (creation node)"},
	    {14, "invalid sector position (sector node)"},
	    {15, "invalid sector position (player game state node)"},
	    {16, "out of bounds train track index"},
	    {17, "out of bounds train config index"},
	    {18, "out of bounds carriage config index"},
	    {19, "mount flag"},
	    {20, "invalid gadget"},
	    {21, "submarine car (sync)"},
	    {22, "submarine car (creation)"},
	    {23, "invalid sector position (camera data node)"},
	    {24, "out of bounds gadget type"},
	    {25, "invalid ped task"},
	    {26, "invalid vehicle task"},
	    {27, "invalid script info"},
	    {28, "invalid interior"},
	    {29, "out of bounds object type"},
	    {30, "incorrect object type"},
	    {31, "vehicle temp action"},
	    {32, "invalid script entity change type"},
	    {33, "rope"},
	    {34, "pop group override"},
	    {35, "invalid world state type"},
	    {36, "remove unarmed"},
	    {37, "out of bounds give control type"},
	    {38, "invalid parachute object type"},
	    {39, "invalid parachute model"},
	    {40, "out of bounds instance id"},
	};
}
