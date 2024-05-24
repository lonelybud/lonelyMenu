#include "backend/player_command.hpp"
#include "core/scr_globals.hpp"
#include "pointers.hpp"
#include "services/notifications/notification_service.hpp"

#include <script/globals/GPBD_FM.hpp>
#include <script/globals/GlobalPlayerBD.hpp>

namespace big
{
	class kick_from_interior : player_command
	{
		using player_command::player_command;

		virtual void execute(player_ptr player) override
		{
			if (scr_globals::gpbd_fm_1.as<GPBD_FM*>()->Entries[player->id()].PropertyData.Index != -1)
				g_notification_service.push_error("Interior Kick Failed", "Need script connection service");
			else
			{
				const size_t arg_count = 9;
				int64_t args[arg_count]{
				    (int64_t)eRemoteEvent::KickFromInterior,
				    (int64_t)self::id,
				    1 << player->id(),
				    (int64_t)scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()
				        ->Entries[player->id()]
				        .SimpleInteriorData.Index,
				    (int64_t)scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()
				        ->Entries[player->id()]
				        .SimpleInteriorData.InstanceId,
				};

				g_pointers->m_gta.m_trigger_script_event(1, args, arg_count, 1 << player->id(), (int)eRemoteEvent::KickFromInterior);
			}
		}
	};

	kick_from_interior g_kick_from_interior("intkick", "Interior Kick", "");
}