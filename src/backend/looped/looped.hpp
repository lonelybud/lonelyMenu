#pragma once

namespace big
{
	class looped
	{
		struct saved_weapon
		{
			Hash hash;
			std::string name;
		};

	public:
		static void update_globals();
		static void hud_disable_input();
		static void system_desync_kick_protection();
		static void death_check();
		static void vehicle_auto_drive();
		static void self_host_notify();
		static void custom_thread();
		static void ptfx_loop();
	};
}
