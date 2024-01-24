#include "backend.hpp"

#include "core/data/misc.hpp"
#include "core/data/session.hpp"
#include "core/scr_globals.hpp"
#include "gta/joaat.hpp"
#include "looped/looped.hpp"
#include "looped_command.hpp"
#include "script.hpp"
#include "script_patches.hpp"
#include "services/bad_players/bad_players.hpp"
#include "services/context_menu/context_menu_service.hpp"
#include "services/custom_teleport/custom_teleport_service.hpp"
#include "services/known_players.hpp"
#include "util/player.hpp"

namespace big
{
	void backend::loop()
	{
		register_script_patches();

		bad_players_nm::load_blocked_list();
		known_player_nm::load_list();
		g_custom_teleport_service.fetch_saved_locations();

		while (g_running)
		{
			looped::update_globals();
			looped::system_desync_kick_protection();
			looped::system_spoofing();

			for (auto command : g_looped_commands)
				if (command->is_enabled())
					command->on_tick();

			script::get_current()->yield();
		}
	}

	void backend::misc_loop()
	{
		LOG(INFO) << "Starting script: Miscellaneous";

		while (g_running)
		{
			looped::hud_disable_input();

			looped::vehicle_auto_drive();

			script::get_current()->yield();
		}
	}

	void backend::players_state()
	{
		LOG(INFO) << "Starting script: Players State";

		while (g_running)
		{
			static bool death_source_identified = false;
			auto m_game_session_state           = gta_util::get_network()->m_game_session_state;

			if (m_game_session_state == 0 || m_game_session_state > 4)
			{
				if (g_local_player && g_local_player->m_player_info->m_game_state == eGameState::Died)
				{
					if (!death_source_identified)
					{
						if (auto ent = PED::GET_PED_SOURCE_OF_DEATH(self::ped); ent)
						{
							death_source_identified = true;
							Ped ped                 = 0;

							if (ENTITY::IS_ENTITY_A_PED(ent))
								ped = ent;
							else if (ENTITY::IS_ENTITY_A_VEHICLE(ent))
								ped = VEHICLE::GET_PED_IN_VEHICLE_SEAT(ent, -1, 0);

							if (ped && PED::IS_PED_A_PLAYER(ped))
								if (auto player = get_player_from_ped(ped))
									LOG(WARNING) << "You got Killed by: " << player->get_name();
						}

						if (g_local_player->m_vehicle)
							g_local_player->m_vehicle->m_door_lock_status = (int)eVehicleLockState::VEHICLELOCK_LOCKED;
					}
				}
				else if (death_source_identified)
					death_source_identified = false;
			}

			if (g_player_service->get_self()->is_host())
			{
				if (!g_session.notified_as_host)
				{
					g_session.notified_as_host = true;
					g_notification_service->push_success("You are host", "", true);
					g_session.next_host_list.delete_plyr(g_player_service->get_self()->id());
				}

				for (auto& [_, player] : g_player_service->players())
					if (!player->has_joined)
						if (auto& entry = scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[player->id()];
						    entry.FreemodeState == eFreemodeState::RUNNING
						    && !(entry.PlayerStateFlags.IsSet(ePlayerStateFlags::kPlayerSwitchStateInClouds)
						        || entry.PlayerStateFlags.IsSet(ePlayerStateFlags::kPlayerSwitchStateDescent)))
							player->has_joined = true;
			}

			script::get_current()->yield();
		}
	}

	void backend::disable_control_action_loop()
	{
		LOG(INFO) << "Starting script: Disable Control Action";

		while (g_running)
		{
			context_menu_service::disable_control_action_loop();

			script::get_current()->yield();
		}
	}

	void backend::tunables_script()
	{
		// g_tunables_service->run_script();
	}
}
