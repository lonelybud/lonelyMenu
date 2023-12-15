#include "backend/bool_command.hpp"
#include "backend/looped_command.hpp"
#include "core/data/self.hpp"
#include "core/scr_globals.hpp"
#include "natives.hpp"

#include <script/globals/GlobalPlayerBD.hpp>

namespace big
{
	class off_radar : looped_command
	{
		using looped_command::looped_command;

		virtual void on_tick() override
		{
			scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[self::id].OffRadarActive = true;
			*scr_globals::freemode_properties.at(58).as<int*>() = NETWORK::GET_NETWORK_TIME() + 1;
		}

		virtual void on_disable() override
		{
			scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[self::id].OffRadarActive = false;
		}
	};

	off_radar g_off_radar("otr", "Off The Radar", "Hides your blip from other players", g_self.off_radar);
}
