#pragma once
#include "core/data/debug.hpp"
#include "core/data/session.hpp"

namespace big
{
	namespace freemode
	{
		void GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH(rage::scrNativeCallContext* src)
		{
			const auto hash = src->get_arg<rage::joaat_t>(0);

			if (g_debug.freemode_hook_G_N_O_T_R_T_S_W_T_H && hash == RAGE_JOAAT("main"))
			{
				src->set_return_value(0);
				return;
			}

			src->set_return_value(SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH(hash));
		}

		inline void IS_PLAYER_PLAYING(rage::scrNativeCallContext* src)
		{
			// block undead OTR
			if (g_session.decloak_players && src->get_arg<Player>(0) != self::id && !NETWORK::NETWORK_IS_ACTIVITY_SESSION())
				src->set_return_value<BOOL>(TRUE);
			else
				src->set_return_value<BOOL>(PLAYER::IS_PLAYER_PLAYING(src->get_arg<Player>(0)));
		};
	}
}