#pragma once
#include "core/data/session.hpp"
#include "native_hooks.hpp"
#include "natives.hpp"
#include "pointers.hpp"

namespace big
{
	namespace network
	{
		inline void NETWORK_SESSION_HOST(rage::scrNativeCallContext* src)
		{
			if (g_session.join_queued)
			{
				g_pointers->m_gta.m_join_session_by_info(*g_pointers->m_gta.m_network, &g_session.info, 0, 1 | 2, nullptr, 0);
				g_session.join_queued = false;
				src->set_return_value<BOOL>(TRUE);
			}
			else
			{
				src->set_return_value<BOOL>(NETWORK::NETWORK_SESSION_HOST(src->get_arg<int>(0), src->get_arg<int>(1), src->get_arg<BOOL>(2)));
			}
		}
	}
}
