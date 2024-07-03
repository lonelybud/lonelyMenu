#pragma once
#include "natives.hpp"
#include "services/tunables/tunables_service.hpp"

namespace big
{
	namespace tunables
	{

		// **************************************************************************** tunable processing
		// pick a value say "m_current_junk_val". Set this as key in temporary map and its map value as tunable hash.
		// also return this value so that it gets saved in tunables memory region overriding existing value in there.

		inline void _NETWORK_GET_TUNABLES_REGISTRATION_INT(rage::scrNativeCallContext* src)
		{
			if (g_tunables_service->is_processing())
			{
				g_tunables_service->m_junk_values.emplace(g_tunables_service->m_current_junk_val, src->get_arg<Hash>(0));
				src->set_return_value<int>(g_tunables_service->m_current_junk_val++);
				return;
			}
			src->set_return_value<int>(NETWORK::_NETWORK_GET_TUNABLES_REGISTRATION_INT(src->get_arg<Hash>(0), src->get_arg<int>(1)));
		};

		inline void _NETWORK_GET_TUNABLES_REGISTRATION_BOOL(rage::scrNativeCallContext* src)
		{
			if (g_tunables_service->is_processing())
			{
				g_tunables_service->m_junk_values.emplace(g_tunables_service->m_current_junk_val, src->get_arg<Hash>(0));
				src->set_return_value<int>(g_tunables_service->m_current_junk_val++);
				return;
			}
			src->set_return_value<BOOL>(NETWORK::_NETWORK_GET_TUNABLES_REGISTRATION_BOOL(src->get_arg<Hash>(0), src->get_arg<BOOL>(1)));
		};

		inline void _NETWORK_GET_TUNABLES_REGISTRATION_FLOAT(rage::scrNativeCallContext* src)
		{
			if (g_tunables_service->is_processing())
			{
				g_tunables_service->m_junk_values.emplace(g_tunables_service->m_current_junk_val, src->get_arg<Hash>(0));
				src->set_return_value<int>(g_tunables_service->m_current_junk_val++);
				return;
			}
			src->set_return_value<float>(NETWORK::_NETWORK_GET_TUNABLES_REGISTRATION_FLOAT(src->get_arg<Hash>(0), src->get_arg<float>(1)));
		};

		//
	}
}