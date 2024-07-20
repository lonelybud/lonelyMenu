#include "backend/looped_command.hpp"
#include "core/data/misc.hpp"
#include "services/tunables/tunables_service.hpp"
#include "core/scr_globals.hpp"

namespace big
{
	class end_passive_time : looped_command
	{
		using looped_command::looped_command;

		virtual void on_tick() override
		{
			// associated with - Passive Mode is disabled as you have recently killed a player. It will be available in
			*g_tunables_service->get_tunable<int*>("VC_PASSIVE_COOLDOWN_KILL"_J) = 0; 
			*scr_globals::passive_1.as<int*>() = 0; // Global_1964652

			 // associated with - Passive Mode will next be available in
			*g_tunables_service->get_tunable<int*>("VC_PASSIVE_COOLDOWN_REENABLE"_J) = 0;
			*scr_globals::passive_2.at(4512).as<int*>() = 0; // Global_2738934.f_4512 

			*g_tunables_service->get_tunable<int*>("VC_PASSIVE_TIME_AFTER_DISABLE"_J) = 0; // time it takes to end current passive mode
		}

		virtual void on_disable() override
		{
			// tuneables_processing.c
			*g_tunables_service->get_tunable<int*>("VC_PASSIVE_COOLDOWN_KILL"_J)      = 120000;
			*g_tunables_service->get_tunable<int*>("VC_PASSIVE_COOLDOWN_REENABLE"_J)  = 300000;
			*g_tunables_service->get_tunable<int*>("VC_PASSIVE_TIME_AFTER_DISABLE"_J) = 30000;
		}
	};

	end_passive_time g_end_passive_time("endpassivetime", "End Passive Time", "", g_misc.end_passive_time);
}