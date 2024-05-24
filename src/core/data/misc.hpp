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

		bool fm_mission_controller_cart_grab = false;
		bool disable_clothing_validation     = false;
		bool apply_outfit_hair               = false;
		bool request_control                 = true;
		bool notify_friend_killed            = true;
		bool ped_ignore                      = false;
	};

	inline g_misc_t g_misc{};
}