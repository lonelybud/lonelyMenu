#include "backend/player_command.hpp"
#include "natives.hpp"
#include "packet.hpp"

namespace big
{
	class break_game : player_command
	{
		using player_command::player_command;

		virtual void execute(player_ptr player) override
		{
			packet pack;
			pack.write_message(rage::eNetMessage::MsgRoamingJoinBubbleAck);
			pack.write(0, 2);
			pack.write(10, 4);
			pack.write(0, 6);
			pack.send(player->get_net_game_player()->m_msg_id);
		}
	};

	break_game g_break_game("breakgame", "Break Game", "Forces the player into a broken singleplayer state");
}