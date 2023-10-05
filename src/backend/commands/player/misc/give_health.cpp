#include "backend/player_command.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "services/pickups/pickup_service.hpp"
#include "util/globals.hpp"

namespace big
{
	class give_health : player_command
	{
		using player_command::player_command;

		virtual CommandAccessLevel get_access_level() override
		{
			return CommandAccessLevel::FRIENDLY;
		}

		virtual void execute(player_ptr player, const command_arguments& _args, const std::shared_ptr<command_context> ctx) override
		{
			g_pickup_service->give_player_health(player->id());
		}
	};

	give_health g_give_health("givehealth", "Give Health", "Gives the player some health", 0);
}