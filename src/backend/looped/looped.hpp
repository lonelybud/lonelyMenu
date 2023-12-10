#pragma once

namespace big
{
	class looped
	{
	public:
		static void hud_disable_input();
		static void update_globals();
		static void system_desync_kick_protection();
		static void system_spoofing();
		static void self_persist_outfit();
	};
}
