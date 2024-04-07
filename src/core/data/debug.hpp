#pragma once

namespace big
{
	inline struct g_debug
	{
		bool freemode_hook_G_N_O_T_R_T_S_W_T_H = false; // https://github.com/lonelybud/lonelyMenu/discussions/44
		bool log_explosion_event               = false;
		bool enable_objects_crash_2            = true;
		bool log_packets                       = false;
		bool log_script_events                 = false;
		bool log_clones                        = false;
		bool use_default_chat                  = false;
	} g_debug{};
}