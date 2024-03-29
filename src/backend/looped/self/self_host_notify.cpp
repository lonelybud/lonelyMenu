#include "backend/looped/looped.hpp"
#include "backend/player_command.hpp"
#include "core/data/session.hpp"
#include "services/players/player_service.hpp"

namespace big
{
	void looped::self_host_notify()
	{
		if (g_player_service->get_self()->is_host() && !g_session.notified_as_host)
		{
			g_session.notified_as_host = true;
			g_notification_service.push_success("You are host", "", true);
			g_session.next_host_list.delete_plyr(g_player_service->get_self()->id());

			// kick all blocked players
			g_player_service->iterate([](const player_entry& player) {
				if (player.second->is_blocked)
					dynamic_cast<player_command*>(command::get("hostkick"_J))->call(player.second);
			});
		}
	}
}