#pragma once

namespace big
{
	inline struct g_debug
	{
		bool disable_population_load_balancing = true;
		bool freemode_hook_G_N_O_T_R_T_S_W_T_H = false; // https://github.com/lonelybud/lonelyMenu/discussions/44
		bool log_explosion_event               = false;
	} g_debug{};
}