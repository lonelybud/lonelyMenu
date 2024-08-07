#include "backend/player_command.hpp"
#include "core/vars.hpp"
#include "gta/net_object_mgr.hpp"
#include "pointers.hpp"
#include "services/notifications/notification_service.hpp"

namespace big
{
	class kick_from_vehicle : player_command
	{
		using player_command::player_command;

		virtual void execute(player_ptr player) override
		{
			auto vehicle = player->get_current_vehicle();

			if (!vehicle || !vehicle->m_net_object)
			{
				// vehicle hasn't synced yet, use TSE
				const size_t arg_count = 10;
				int64_t args[arg_count] = {(int64_t)eRemoteEvent::VehicleKick, self::id, 1 << player->id(), 0, 0, 0, 0, 0, 0, 0};

				g_pointers->m_gta.m_trigger_script_event(1, args, arg_count, 1 << player->id(), (int)eRemoteEvent::VehicleKick);

				g_notification_service.push_error("Vehicle Kick", std::format("TSE Kick sent to {}", player->m_name));
			}
			else
			{
				// use a private method to kick player from vehicle
				(*g_pointers->m_gta.m_network_object_mgr)
				    ->ChangeOwner(vehicle->m_net_object, g_player_service->get_self()->get_net_game_player(), 0);

				g_notification_service.push_success("Vehicle Kick", std::format("Kick sent to {}", player->m_name));
			}
		}
	};

	kick_from_vehicle g_kick_from_vehicle("vehkick", "Vehicle Kick", "");
}