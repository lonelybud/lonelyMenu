#pragma once

namespace big
{
	inline struct g_debug
	{
		bool log_explosion_event      = false;
		bool log_ptfx_event           = false;
		bool log_sound_event          = false;
		bool log_packets              = false;
		bool log_script_events        = false;
		bool log_clones               = false;
		bool log_vehicle_clones       = false;
		bool log_ped_clones           = false;
		bool log_CPedHealthDataNode   = false; // spam kill feed detection
		bool log_object_clones        = false;
		bool log_CVehicleTaskDataNode = false;
	} g_debug{};
}