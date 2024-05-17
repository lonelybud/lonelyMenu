#pragma once
#include "natives.hpp"
#include "core/data/bad_scripts.hpp"

namespace big
{
	namespace am_launcher
	{
		static void START_NEW_SCRIPT_WITH_ARGS(rage::scrNativeCallContext* src)
		{
			const char* const name = src->get_arg<const char*>(0);

			if (!NETWORK::NETWORK_IS_ACTIVITY_SESSION() && bad_script_hashes.contains(rage::joaat(name)))
			{
				LOG(WARNING) << "Blocked bad script from starting -> " << name;
				src->set_return_value<int>(0);
				return;
			}

			Any* args           = src->get_arg<Any*>(1);
			const int argc      = src->get_arg<int>(2);
			const int stackSize = src->get_arg<int>(3);

			src->set_return_value<int>(SYSTEM::START_NEW_SCRIPT_WITH_ARGS(name, args, argc, stackSize));
		}
	}
}