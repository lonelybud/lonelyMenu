#pragma once
#include "core/data/bad_scripts.hpp"
#include "natives.hpp"
#include "services/notifications/notification_service.hpp"
#include "util/scripts.hpp"

namespace big
{
	namespace am_launcher
	{
		static void START_NEW_SCRIPT_WITH_ARGS(rage::scrNativeCallContext* src)
		{
			const char* const name = src->get_arg<const char*>(0);

			if (!NETWORK::NETWORK_IS_ACTIVITY_SESSION() && bad_script_hashes.contains(rage::joaat(name)))
			{
				auto freemode_host      = scripts::get_script_handler_net_comp("freemode"_J)->get_host();
				auto am_launcher_host   = scripts::get_script_handler_net_comp("am_launcher"_J)->get_host();
				auto fmmc_launcher_host = scripts::get_script_handler_net_comp("fmmc_launcher"_J)->get_host();

				g_notification_service.push_warning("Bad Script", std::format("Blocked script {}", name), true);
				LOGF(WARNING,
				    "freemode: {}, am_launcher: {}, fmmc_launcher: {}",
				    freemode_host ? freemode_host->get_name() : "?",
				    am_launcher_host ? am_launcher_host->get_name() : "?",
				    fmmc_launcher_host ? fmmc_launcher_host->get_name() : "?");

				src->set_return_value<int>(0);
				return;
			}

			Any* args     = src->get_arg<Any*>(1);
			int argc      = src->get_arg<int>(2);
			int stackSize = src->get_arg<int>(3);

			src->set_return_value<int>(SYSTEM::START_NEW_SCRIPT_WITH_ARGS(name, args, argc, stackSize));
		}
	}
}