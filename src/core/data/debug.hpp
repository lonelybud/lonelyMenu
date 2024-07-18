#pragma once

namespace big
{
	inline struct g_debug
	{
		bool log_packets       = false; // log net msgs
		bool log_script_events = false; // log scripted events
		bool log_clones        = false; // log clones
	} g_debug{};
}