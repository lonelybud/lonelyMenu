#pragma once

namespace big
{
	inline struct g_session
	{
		uint64_t custom_host_token = 0;
		bool lock_session          = false;
	} g_session{};
}