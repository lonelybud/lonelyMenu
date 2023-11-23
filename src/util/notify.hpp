#pragma once
#include "core/data/reactions.hpp"
#include "gta/enums.hpp"
#include "natives.hpp"
#include "network/CNetGamePlayer.hpp"
#include "network/ChatData.hpp"
#include "pointers.hpp"
#include "script.hpp"
#include "services/notifications/notification_service.hpp"
#include "services/players/player_service.hpp"

#include <script/HudColor.hpp>

namespace big::notify
{
	inline void crash_blocked(player_ptr plyr, const char* crash_detail)
	{
		if (plyr)
		{
			reaction crash{"Crash", std::format("X: Blocked {} crash from {}", crash_detail, plyr->get_name()).c_str()};
			crash.process(plyr, false, Infraction::TRIED_CRASH_PLAYER, true);
		}
		else
			g_notification_service->push_error("Protections", std::format("X: Blocked {} crash from unknown player", crash_detail), true);
	}
}