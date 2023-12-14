#include "backend/player_command.hpp"
#include "core/scr_globals.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "services/notifications/notification_service.hpp"

namespace big
{
	class null_function_kick : player_command
	{
		using player_command::player_command;

		virtual void execute(player_ptr player) override
		{
			if (!player)
				return;

			g_notification_service->push_success("Kick", std::format("Null Function kick to {}", player->get_name()), true);

			const size_t arg_count  = 16;
			int64_t args[arg_count] = {(int64_t)eRemoteEvent::InteriorControl, (int64_t)self::id, 1 << player->id(), (int64_t)(int)-1};

			g_pointers->m_gta.m_trigger_script_event(1, args, arg_count, 1 << player->id(), (int)eRemoteEvent::InteriorControl);
		}
	};

	null_function_kick g_null_function_kick("nfkick", "Null Function Kick", "It may take around 15 seconds for the player to actually leave the session");
}