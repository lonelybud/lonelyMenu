#include "backend/player_command.hpp"
#include "gta/net_object_mgr.hpp"
#include "natives.hpp"
#include "pointers.hpp"

namespace big
{
	class kick_from_vehicle : player_command
	{
		using player_command::player_command;

		virtual void execute(player_ptr player) override
		{
			auto vehicle = player->get_current_vehicle();
			// use a private method to kick player from vehicle
			(*g_pointers->m_gta.m_network_object_mgr)->ChangeOwner(vehicle->m_net_object, g_player_service->get_self()->get_net_game_player(), 0);
		}
	};

	kick_from_vehicle g_kick_from_vehicle("vehkick", "Vehicle Kick", "");
}