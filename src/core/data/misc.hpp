#pragma once

namespace big
{
	struct g_misc_t
	{
		struct custom_time
		{
			bool override_time = false;
			int hour{}, minute{}, second{};
		} custom_time;

		bool disable_clothing_validation = false;
		bool apply_outfit_hair           = false;
		bool notify_friend_killed        = true;
		bool ped_ignore                  = false;
		bool end_passive_time            = false;

		bool log_explosion_event = false; // help identify blamed explosions
		bool log_ptfx_event      = false; // help identify unusual ptfx events which are not catched by m_ptfx_ratelimit
		bool log_sound_event     = false; // help identify sound spams which are not caught
		bool log_vehicle_clones  = false; // log vehicles sync
		bool log_ped_clones      = false; // log peds sync
		bool log_CPedHealthDataNode   = false; // spam kill feed detection
		bool log_object_clones        = false; // log object sync
		bool log_CVehicleTaskDataNode = false; // help identify modders creating vehicles and assigning tasks to them
	};

	inline g_misc_t g_misc{};
}