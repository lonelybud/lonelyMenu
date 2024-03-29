#include "backend/looped_command.hpp"
#include "core/data/self.hpp"
#include "natives.hpp"
#include "services/players/player_service.hpp"

namespace big
{
	class spectate : looped_command
	{
		using looped_command::looped_command;

		player_ptr target_player;

		virtual void on_enable() override
		{
			if (SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH("maintransition"_J) != 0)
			{
				g_self.spectating = false;
				return;
			}

			target_player = g_player_service->get_selected();

			const auto target_ped = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(target_player->id());

			NETWORK::NETWORK_SET_IN_SPECTATOR_MODE(TRUE, target_ped);
			NETWORK::NETWORK_OVERRIDE_RECEIVE_RESTRICTIONS_ALL(FALSE);
			HUD::SET_MINIMAP_IN_SPECTATOR_MODE(TRUE, target_ped);
			HUD::SET_BLIP_ALPHA(HUD::GET_MAIN_PLAYER_BLIP_ID(), 255);
			STREAMING::SET_FOCUS_ENTITY(target_ped);
		}

		virtual void on_tick() override
		{
			if (!target_player->is_valid() || g_player_service->get_selected() != target_player)
			{
				g_self.spectating = false;
				on_disable();
			}
		}

		virtual void on_disable() override
		{
			NETWORK::NETWORK_SET_IN_SPECTATOR_MODE(FALSE, -1);
			NETWORK::NETWORK_OVERRIDE_RECEIVE_RESTRICTIONS_ALL(FALSE);
			HUD::SET_MINIMAP_IN_SPECTATOR_MODE(FALSE, -1);
			HUD::SET_BLIP_ALPHA(HUD::GET_MAIN_PLAYER_BLIP_ID(), 255);
			STREAMING::SET_FOCUS_ENTITY(self::ped);
		}
	};

	spectate g_spectate("spectate", "Spectate", "", g_self.spectating);
}