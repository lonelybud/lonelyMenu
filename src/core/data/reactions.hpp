#pragma once
#include "backend/reactions/reaction.hpp"

namespace big
{
	inline struct g_reactions_t
	{
		reaction end_session_kick{"End Session Kick", "Blocked End Session Kick from %s"};
		reaction network_bail{"Network Bail", "Blocked Network Bail from %s"};
		reaction null_function_kick{"Null Function Kick", "Blocked Null Function Kick from %s"};
		reaction kick_vote{"Kick Vote", "%s is voting to kick you!"};
		reaction oom_kick{"OOM Kick", "Blocked OOM Kick from %s"};
		reaction crash{"Crash", "Blocked Crash from %s"};
		reaction stand_user_crash{"Crash", "Blocked Stand User Crash from %s"};
		reaction report{"Report", "Blocked Report from %s"};

		reaction gta_banner{"GTA Banner", "Blocked GTA Banner from %s"}; // please don't enable this
		reaction tse_freeze{"TSE Freeze", "Blocked TSE Freeze from %s"};
		reaction tse_sender_mismatch{"TSE Sender Mismatch", "Blocked TSE Sender Mismatch from %s"};

		reaction game_anti_cheat_modder_detection{"Anti-Cheat Modder Detection", "%s is detected as a modder by anti-cheat!"};
		reaction modder_detection{"Modder Detection", "%s is detected as a modder!"};

		reaction ceo_kick{"CEO Kick", "Blocked CEO Kick from %s"};
		reaction ceo_money{"CEO Money", "Blocked CEO Money from %s"};
		reaction clear_wanted_level{"Clear Wanted Level", "Blocked Clear Wanted Level from %s"};
		reaction fake_deposit{"Fake Deposit", "Blocked Fake Deposit from %s"};
		reaction force_mission{"Force Mission", "Blocked Force Mission from %s"};
		reaction force_teleport{"Force Teleport", "Blocked Force Teleport from %s"};
		reaction kick_from_interior{"Kick From Interior", "Blocked Kick From Interior from %s"};
		reaction mc_teleport{"MC Teleport", "Blocked MC Teleport from %s"};
		reaction personal_vehicle_destroyed{"Personal Vehicle Destroyed", "Blocked Personal Vehicle Destroyed from %s"};
		reaction destroy_personal_vehicle{"Destroy Personal Vehicle", "Blocked Destroy Personal Vehicle from %s"};
		reaction give_collectible{"Give Collectible", "Blocked Give Collectible from %s"};
		reaction remote_off_radar{"Remote Off Radar", "Blocked Remote Off Radar from %s"};
		reaction rotate_cam{"Rotate Cam", "Blocked Rotate Cam from %s"};
		reaction send_to_cutscene{"Send To Cutscene", "Blocked Send To Cutscene from %s"};
		reaction send_to_location{"Send To Location", "Blocked Send To Location from %s"};
		reaction send_to_interior{"Send To Interior", "Blocked Send To Interior from %s"};
		reaction sound_spam{"Sound Spam", "Blocked Sound Spam from %s"};
		reaction spectate_notification{"Spectate Notification", "Blocked Spectate Notification from %s"};
		reaction transaction_error{"Transaction Error", "Blocked Transaction Error from %s"};
		reaction vehicle_kick{"Vehicle Kick", "Blocked Vehicle Kick from %s"};
		reaction teleport_to_warehouse{"Teleport To Warehouse", "Blocked Teleport To Warehouse from %s"};
		reaction start_activity{"Start Activity", "Blocked Start Activity from %s"};
		reaction start_script{"Start Script", "Blocked Start Script from %s"};
		reaction trigger_business_raid{"Trigger Business Raid", "Blocked Trigger Business Raid from %s"};
		reaction turn_into_beast{"Turn Into Beast", "Blocked Turn Into Beast from %s"};
		reaction remote_wanted_level{"Remote Wanted Level", "Blocked Remote Wanted Level from %s"};
		reaction clear_ped_tasks{"Clear Ped Tasks", "Blocked Clear Ped Tasks from %s"};
		reaction remote_ragdoll{"Remote Ragdoll", "Blocked Remote Ragdoll from %s"};
		reaction report_cash_spawn{"Cash Spawn", "%s is spawning cash!"};
		reaction request_control_event{"Request Control Event", "Blocked Request Control Event from %s"};
		reaction spectate{"Spectate", "%s is spectating you"};
	} g_reactions{};
}
