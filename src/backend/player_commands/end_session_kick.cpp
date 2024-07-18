#include "backend/player_command.hpp"
#include "core/scr_globals.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "script.hpp"
#include "services/notifications/notification_service.hpp"
#include "util/scripts.hpp"

namespace big
{
	class end_session_kick : player_command
	{
		using player_command::player_command;

		virtual void execute(player_ptr player) override
		{
			if (!player)
				return;
			if (!scripts::is_local_player_host("freemode"_J))
			{
				g_notification_service.push_error("End Session Kick", "Be freemode host first!", true);
				return;
			}

			g_notification_service.push_success("Kick", std::format("End session kick to {}", player->m_name, true));
			player->trigger_end_session_kick = true;
			*scr_globals::gsbd.as<int*>() = (int)(__rdtsc() % 50000) + 6; // making the game trigger the broadcast is a bit difficult and requires a little bit of tampering with the value and some luck
			script::get_current()->yield(5s);
			*scr_globals::gsbd.as<int*>() = 4;
		}
	};

	end_session_kick g_end_session_kick("endkick", "End Session Kick", "This may take around 10 seconds to kick the player");
}
