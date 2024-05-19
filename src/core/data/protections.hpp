#pragma once

namespace big
{
	inline struct g_protections_t
	{
		struct script_events
		{
			bool ceo_money                  = true;
			bool clear_wanted_level         = true;
			bool force_mission              = true;
			bool force_teleport             = true;
			bool gta_banner                 = true;
			bool mc_teleport                = true;
			bool personal_vehicle_destroyed = true;
			bool remote_off_radar           = true;
			bool rotate_cam                 = true;
			bool send_to_cutscene           = true;
			bool send_to_location           = true;
			bool sound_spam                 = true;
			bool spectate                   = true;
			bool give_collectible           = true;
			bool vehicle_kick               = true;
			bool teleport_to_warehouse      = true;
			bool start_activity             = true;
			bool send_sms                   = true;
		} script_events{};

		bool request_control_event = true;
		bool clear_ped_tasks       = true;
		bool receive_pickup        = false;
		bool stop_traffic          = true;
		bool desync_kick           = false;
		bool cage_protection       = false;
	} g_protections{};
}