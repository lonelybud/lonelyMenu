#pragma once
#include "command.hpp"
#include "fiber_pool.hpp"
#include "services/players/player_service.hpp"

namespace big
{
	class player_command : public command
	{
	public:
		player_command(const std::string& name, const std::string& label, const std::string& description) :
		    command(name, label, description)
		{
		}

		virtual void execute(player_ptr player) = 0;

		virtual void call(player_ptr player)
		{
			g_fiber_pool->queue_job([this, player] {
				if (player && player->is_valid())
					execute(player);
			});
		}
	};
}