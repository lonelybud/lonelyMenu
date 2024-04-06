#pragma once
#include "backend/looped/looped.hpp"
#include "core/settings.hpp"
#include "gta/weapons.hpp"
#include "services/bad_players/bad_players.hpp"
#include "services/custom_chat_buffer.hpp"
#include "services/custom_teleport/custom_teleport_service.hpp"
#include "services/gta_data/gta_data_service.hpp"
#include "services/known_players/known_players.hpp"
#include "services/mobile/mobile_service.hpp"
#include "services/players/player_service.hpp"
#include "thread_pool.hpp"

namespace big
{
	void looped::custom_thread()
	{
		g_thread_pool->push([] {
			while (!g_running)
				std::this_thread::yield();

			g_bad_players_service.load_blocked_list();
			g_known_players_service.load_list();
			g_custom_teleport_service.fetch_saved_locations();

			int last_pv_len{}, last_wp_len{}, bad_players_count = g_bad_players_service.save_count,
			                                  known_players_count = g_known_players_service.save_count;

			while (g_running)
			{
				std::this_thread::sleep_for(5000ms);

				// auto save current settings to disk
				g_menu_settings.attempt_save();

				// auto flush chat to disk
				g_custom_chat_buffer.flush_buffer();

				// auto save bad players
				if (auto bpc = g_bad_players_service.save_count; bad_players_count != bpc)
				{
					g_bad_players_service.save_blocked_list();
					LOG(VERBOSE) << "Saved " << bpc - bad_players_count << " bad players";
					bad_players_count = bpc;
				}

				// auto save known players
				if (auto kpc = g_known_players_service.save_count; known_players_count != kpc)
				{
					g_known_players_service.save_list();
					LOG(VERBOSE) << "Saved " << kpc - known_players_count << " known players";
					known_players_count = kpc;
				}

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
