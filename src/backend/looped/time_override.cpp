#include "backend/looped_command.hpp"
#include "core/data/session.hpp"
#include "natives.hpp"

namespace big
{
	class time_override : looped_command
	{
		using looped_command::looped_command;

		virtual void on_tick() override
		{
			NETWORK::NETWORK_OVERRIDE_CLOCK_TIME(g_session.custom_time.hour,
			    g_session.custom_time.minute,
			    g_session.custom_time.second);
		}

		virtual void on_disable() override
		{
			NETWORK::NETWORK_CLEAR_CLOCK_TIME_OVERRIDE();
		}
	};

	time_override g_time_override("timeoverride", "Override Time", "This is local.", g_session.custom_time.override_time);
}