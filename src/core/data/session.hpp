#pragma once

namespace big
{
	inline struct g_session
	{
		uint64_t smallest_host_token = std::numeric_limits<uint64_t>::max();
		uint64_t custom_host_token   = 0;
		bool lock_session            = false;
	} g_session{};
}