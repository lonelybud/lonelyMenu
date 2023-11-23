#pragma once

namespace big
{
	inline struct g_session
	{
		bool force_session_host = false;
		bool lock_session       = false;
	} g_session{};
}