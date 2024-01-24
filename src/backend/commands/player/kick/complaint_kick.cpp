#include "backend/player_command.hpp"
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
			g_notification_service->push_success("Kick", std::format("Desync kick to {}", player->get_name()), true);

			if (gta_util::get_network()->m_game_session_ptr->is_host())
			{
				gta_util::get_network()->m_game_complaint_mgr.raise_complaint(player->m_host_token);
				return;
			}

			g_player_service->m_player_to_use_complaint_kick = player;
		}
	};

	complaint_kick g_complaint_kick("desync", "Desync Kick", "This may take around 10 seconds to kick the player. Does not work against the host"); // this is pretty much desync except we don't actually remove the player from CNetworkPlayerMgr
}