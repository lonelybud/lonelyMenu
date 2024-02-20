#pragma once
#include "backend/bool_command.hpp"
#include "backend/player_command.hpp"
#include "core/data/auto_drive.hpp"
#include "core/data/context_menu.hpp"
#include "core/data/hud.hpp"
#include "core/data/self.hpp"
#include "core/data/session.hpp"
#include "core/enums.hpp"
#include "core/scr_globals.hpp"
#include "core/settings.hpp"
#include "gta/enums.hpp"
#include "gta/weapons.hpp"
#include "gta_util.hpp"
#include "natives.hpp"
#include "services/custom_chat_buffer.hpp"
#include "services/gta_data/gta_data_service.hpp"
#include "services/mobile/mobile_service.hpp"
#include "services/players/player_service.hpp"
#include "thread_pool.hpp"
#include "util/blip.hpp"
#include "util/player.hpp"

#include <network/CCommunications.hpp>
#include <network/CNetworkPlayerMgr.hpp>
#include <script/globals/GlobalPlayerBD.hpp>

namespace big::looped
{
	struct saved_weapon
	{
		Hash hash;
		std::string name;
	};

	static std::unordered_map<AutoDriveStyle, int> driving_style_flags = {{AutoDriveStyle::LAW_ABIDING, 443}, {AutoDriveStyle::THE_ROAD_IS_YOURS, 787004}};

	inline void update_globals()
	{
		auto network_player_mgr = gta_util::get_network_player_mgr();

		if (!network_player_mgr || !network_player_mgr->m_local_net_player || network_player_mgr->m_local_net_player->m_player_id == -1)
			self::id = 0;
		else
			self::id = network_player_mgr->m_local_net_player->m_player_id;

		g_local_player = gta_util::get_local_ped();

		auto m_game_session_state = gta_util::get_network()->m_game_session_state;

		if (m_game_session_state == 0 || m_game_session_state > 4)
		{
			self::ped = PLAYER::PLAYER_PED_ID();

			self::pos = ENTITY::GET_ENTITY_COORDS(self::ped, 0);

			if (PED::IS_PED_IN_ANY_VEHICLE(self::ped, 0))
			{
				if (!self::veh)
					self::veh = PED::GET_VEHICLE_PED_IS_IN(self::ped, 0);
			}
			else
				self::veh = 0;
		}
	}

	inline void hud_disable_input()
	{
		if (g_hud.typing)
		{
			PAD::DISABLE_ALL_CONTROL_ACTIONS(0);
			g_hud.typing--;
		}
	}

	inline void system_desync_kick_protection()
	{
		memset(&gta_util::get_network()->m_game_complaint_mgr.m_host_tokens_complained, 0, 64 * sizeof(uint64_t));

		if (!g_player_service->m_player_to_use_complaint_kick
		    || !g_player_service->m_player_to_use_complaint_kick->get()->get_net_data())
			gta_util::get_network()->m_game_complaint_mgr.m_num_tokens_complained = 0;
		else
		{
			gta_util::get_network()->m_game_complaint_mgr.m_num_tokens_complained = 1;
			gta_util::get_network()->m_game_complaint_mgr.m_host_tokens_complained[0] =
			    g_player_service->m_player_to_use_complaint_kick->get()->m_host_token;
		}

		auto old = gta_util::get_network()->m_game_complaint_mgr.m_host_token;

		if (gta_util::get_network()->m_game_session_state > 3 && gta_util::get_network()->m_game_session_state < 6
		    && g_player_service->m_player_to_use_complaint_kick && g_player_service->get_self()->is_valid()
		    && !g_player_service->get_self()->is_host())
		{
			g_pointers->m_gta.m_reset_network_complaints(&gta_util::get_network()->m_game_complaint_mgr);
		}
	}

	inline void host_token_spoofing()
	{
		static uint64_t host_token = 0;

		if (!g_session.orig_host_token && *g_pointers->m_gta.m_host_token)
		{
			g_session.orig_host_token = host_token = g_session.host_token = *g_pointers->m_gta.m_host_token;
			LOG(VERBOSE) << "Your host token is: " << *g_pointers->m_gta.m_host_token;
		}

		if (host_token != g_session.host_token && gta_util::get_network()->m_game_session_state == 0)
		{
			host_token = g_session.host_token;

			LOG(VERBOSE) << "Last host token: " << *g_pointers->m_gta.m_host_token;

			if (host_token == g_session.orig_host_token)
				LOG(VERBOSE) << "Using original host token: " << host_token;
			else
				LOG(VERBOSE) << "Using custom host token: " << host_token;

			*g_pointers->m_gta.m_host_token = host_token;

			if (gta_util::get_network()->m_game_session_ptr)
				gta_util::get_network()->m_game_session_ptr->m_local_player.m_player_data.m_host_token = host_token;

			g_pointers->m_gta.m_profile_gamer_info->m_host_token                                       = host_token;
			g_pointers->m_gta.m_player_info_gamer_info->m_host_token                                   = host_token;
			(*g_pointers->m_gta.m_communications)->m_voice.m_connections[0]->m_gamer_info.m_host_token = host_token;

			if (g_local_player && g_local_player->m_player_info)
				g_local_player->m_player_info->m_net_player_data.m_host_token = host_token;
		}
	}

	inline void death_check()
	{
		if (g_local_player->m_player_info->m_game_state == eGameState::Died)
		{
			if (g_self.spectating)
			{
				g_self.spectating = false;
				dynamic_cast<bool_command*>(command::get(RAGE_JOAAT("spectate")))->refresh();
			}
			if (g_self.free_cam)
			{
				g_self.free_cam = false;
				dynamic_cast<bool_command*>(command::get(RAGE_JOAAT("freecam")))->refresh();
			}
			if (g_context_menu.enabled)
				g_context_menu.enabled = false;
		}
	}

	// inline void death_blame()
	// {
	// 	static bool death_source_identified = false;
	// 	auto m_game_session_state           = gta_util::get_network()->m_game_session_state;

	// 	if ((m_game_session_state == 0 || m_game_session_state > 4) && g_local_player->m_player_info->m_game_state == eGameState::Died)
	// 	{
	// 		if (death_source_identified)
	// 			return;

	// 		if (auto ent = PED::GET_PED_SOURCE_OF_DEATH(self::ped); ent)
	// 		{
	// 			death_source_identified = true;
	// 			Ped ped                 = 0;

	// 			if (ENTITY::IS_ENTITY_A_PED(ent))
	// 				ped = ent;
	// 			else if (ENTITY::IS_ENTITY_A_VEHICLE(ent))
	// 				ped = VEHICLE::GET_PED_IN_VEHICLE_SEAT(ent, -1, 0);

	// 			if (ped && PED::IS_PED_A_PLAYER(ped))
	// 				if (auto player = get_player_from_ped(ped))
	// 					LOG(WARNING) << "You got Killed by: " << player->get_name();
	// 		}

	// 		if (g_local_player->m_vehicle)
	// 			g_local_player->m_vehicle->m_door_lock_status = (int)eVehicleLockState::VEHICLELOCK_LOCKED;
	// 	}
	// 	else if (death_source_identified)
	// 		death_source_identified = false;
	// }

	inline void vehicle_auto_drive()
	{
		static int current_driving_flag = driving_style_flags[AutoDriveStyle::LAW_ABIDING];
		static float current_speed      = 8;

		static bool has_driving_settings_changed = false;

		// start driving if destination is there
		if (g_auto_drive.auto_drive_destination != AutoDriveDestination::STOPPED)
		{
			if (!self::veh)
			{
				g_auto_drive.auto_drive_destination = AutoDriveDestination::STOPPED;
				has_driving_settings_changed        = false;
				g_auto_drive.is_auto_driving        = false;
			}

			// check for changing driving settings
			if (current_driving_flag != driving_style_flags[g_auto_drive.auto_drive_style]
			    || current_speed != g_auto_drive.auto_drive_speed)
			{
				current_driving_flag         = driving_style_flags[g_auto_drive.auto_drive_style];
				current_speed                = g_auto_drive.auto_drive_speed;
				has_driving_settings_changed = true;
			}

			if (!g_auto_drive.is_auto_driving)
			{
				auto location = g_auto_drive.auto_drive_destination == AutoDriveDestination::OBJECTITVE ? blip::get_objective_location() : blip::get_blip_location((int)BlipIcons::Waypoint);

				if (location.has_value())
				{
					auto waypoint = location.value();
					TASK::TASK_VEHICLE_DRIVE_TO_COORD_LONGRANGE(self::ped, self::veh, waypoint.x, waypoint.y, waypoint.z, current_speed, current_driving_flag, 20);
					g_auto_drive.is_auto_driving = true;
				}
				else
				{
					g_auto_drive.auto_drive_destination = AutoDriveDestination::STOPPED;
					has_driving_settings_changed        = false;
				}
			}
			else
			{
				bool interupted = (PAD::IS_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_VEH_MOVE_LEFT_ONLY) || PAD::IS_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_VEH_MOVE_RIGHT_ONLY) || PAD::IS_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_VEH_ACCELERATE) || PAD::IS_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_VEH_BRAKE) || PAD::IS_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_VEH_EXIT) || PAD::IS_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_VEH_HANDBRAKE));

				if (has_driving_settings_changed || interupted)
				{
					TASK::CLEAR_PRIMARY_VEHICLE_TASK(self::veh);
					TASK::CLEAR_PED_TASKS(self::ped);

					has_driving_settings_changed = false;
					g_auto_drive.is_auto_driving = false; // start driving again in next tick if !interupted
				}

				if (interupted)
					g_auto_drive.auto_drive_destination = AutoDriveDestination::STOPPED;
			}
		}
	}

	inline void self_host_notify()
	{
		if (g_player_service->get_self()->is_host() && !g_session.notified_as_host)
		{
			g_session.notified_as_host = true;
			g_notification_service->push_success("You are host", "", true);
			g_session.next_host_list.delete_plyr(g_player_service->get_self()->id());

			// kick all blocked players
			g_player_service->iterate([](const player_entry& player) {
				if (player.second->is_blocked)
					dynamic_cast<player_command*>(command::get(RAGE_JOAAT("hostkick")))->call(player.second);
			});
		}
	}

	inline void custom_thread()
	{
		g_thread_pool->push([] {
			int last_pv_len{}, last_wp_len{};

			while (!g_running)
				std::this_thread::yield();

			while (g_running)
			{
				std::this_thread::sleep_for(5000ms);

				// auto save current settings to disk
				g_menu_settings.attempt_save();

				// auto flush chat to disk
				g_custom_chat_buffer.flush_buffer();

				if (*g_pointers->m_gta.m_is_session_started)
				{
					// refresh pvs
					g_mobile_service->register_vehicles();

					// auto save pv list to disk
					if (last_pv_len != g_mobile_service->personal_vehicles.size())
					{
						last_pv_len = g_mobile_service->personal_vehicles.size();

						std::ofstream pv_list(
						    g_file_manager
						        .get_project_file(std::format("./pv_list_{}.txt", g_player_service->get_self()->get_name()))
						        .get_path(),
						    std::ios_base::out | std::ios_base::trunc);
						for (const auto& it : g_mobile_service->personal_vehicles)
							pv_list << it.first << std::endl;
						pv_list.close();

						LOG(VERBOSE) << "PVs save success!";
					}

					// auto save current weapon loadout when in freemode
					if (!NETWORK::NETWORK_IS_ACTIVITY_SESSION())
					{
						std::vector<saved_weapon> weapons;
						for (const auto& [name, weapon] : g_gta_data_service->weapons())
							if (weapon.m_hash != WEAPON_UNARMED && WEAPON::HAS_PED_GOT_WEAPON(self::ped, weapon.m_hash, FALSE))
								weapons.push_back({weapon.m_hash, name});

						if (last_wp_len != weapons.size())
						{
							last_wp_len = weapons.size();

							std::ofstream wp_list(
							    g_file_manager
							        .get_project_file(std::format("./wp_list_{}.txt", g_player_service->get_self()->get_name()))
							        .get_path(),
							    std::ios_base::out | std::ios_base::trunc);
							for (const auto& w : weapons)
								wp_list << w.name << " " << w.hash << std::endl;
							wp_list.close();

							LOG(VERBOSE) << "Weapons save success!";
						}
					}
				}
			}
		});
	}
}
