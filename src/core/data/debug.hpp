#pragma once

namespace big
{
	inline struct g_debug
	{
		bool log_explosion_event    = false;
		bool log_ptfx_event         = false;
		bool enable_objects_crash_2 = true;
		bool log_packets            = false;
		bool log_script_events      = false;
		bool log_clones             = false;
		// bool use_default_chat                  = false;

		bool log_vehicle_clones = false;
		bool log_ped_clones     = false;
	} g_debug{};
}