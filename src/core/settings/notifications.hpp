#pragma once

namespace big
{
	inline struct g_notifications_t
	{
		struct pair
		{
			bool log    = false;
			bool notify = false;

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(pair, log, notify)
		};

		pair player_join, player_leave{};

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(g_notifications_t, player_join, player_leave)
	} g_notifications{};
}