#include "backend/player_command.hpp"
#include "core/data/desync_kick.hpp"
#include "gta_util.hpp"
#include "pointers.hpp"
#include "services/notifications/notification_service.hpp"

namespace big
{
	class complaint_kick : player_command
	{
		using player_command::player_command;

		virtual void execute(player_ptr player) override
		{
			if (player && player->is_valid())
			{
				auto rockstar_id = player->get_net_data()->m_gamer_handle.m_rockstar_id;
				if (g_desync_kick_players.find(rockstar_id) == g_desync_kick_players.end())
				{
					g_desync_kick_players[rockstar_id] = player;
					g_notification_service.push_success("Kick", std::format("Desync kick to {}", player->m_name), true);
				}
			}
		}
	};

	complaint_kick g_complaint_kick("desync", "Desync Kick", "This may take around 10 seconds to kick the player. Does not work against the host"); // this is pretty much desync except we don't actually remove the player from CNetworkPlayerMgr
}