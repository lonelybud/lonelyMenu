#include "core/data/language_codes.hpp"
#include "core/data/protections.hpp"
#include "core/scr_globals.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "services/bad_players/bad_players.hpp"
#include "services/gta_data/gta_data_service.hpp"
#include "services/gui/gui_service.hpp"
#include "services/known_players.hpp"
#include "services/notifications/notification_service.hpp"
#include "services/vehicle/persist_car_service.hpp"
#include "util/globals.hpp"
#include "util/player.hpp"
#include "util/scripts.hpp"
#include "util/teleport.hpp"
#include "util/vehicle.hpp"
#include "views/view.hpp"

#include <network/netConnection.hpp>
#include <script/globals/GPBD_FM.hpp>
#include <script/globals/GPBD_FM_3.hpp>
#include <script/globals/GlobalPlayerBD.hpp>
#include <ui/CBlipList.hpp>
#include <vehicle/CVehicleModelInfo.hpp>

namespace big
{
	static constexpr char ip_viewer_link[] = "https://iplogger.org/ip-tracker/?ip=";

	static inline const char* get_nat_type_str(int type)
	{
		switch (type)
		{
		case 1: return "open";
		case 2: return "moderate";
		case 3: return "strict";
		default: return "unkown";
		}
	}

	static inline const char* get_freemode_state(eFreemodeState type)
	{
		switch (type)
		{
		case eFreemodeState::NONE: return "None";
		case eFreemodeState::RUNNING: return "Running";
		case eFreemodeState::CLOSING: return "Closing";
		default: return "unkown";
		}
	}

	static inline const char* get_player_state(SCR_BITSET<ePlayerStateFlags> type)
	{
		if (type.IsSet(ePlayerStateFlags::kPlayerSwitchStateInClouds))
			return "clouds";
		if (type.IsSet(ePlayerStateFlags::kPlayerSwitchStateDescent))
			return "descending";
		return "Unknown";
	}

	static big::player_ptr last_selected_player;
	static uint64_t rockstar_id;

	static void extra_info_button(player_ptr current_player)
	{
		components::options_modal(
		    "Extra Info",
		    [current_player] {
			    if (auto id = current_player->id(); id != -1)
			    {
				    auto& gpbd_fm_3       = scr_globals::gpbd_fm_3.as<GPBD_FM_3*>()->Entries[id];
				    auto& gpbd_fm_1       = scr_globals::gpbd_fm_1.as<GPBD_FM*>()->Entries[id];
				    auto& globalplayer_bd = scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[id];

				    auto& stats       = gpbd_fm_1.PlayerStats;
				    auto& boss_goon   = gpbd_fm_3.BossGoon;
				    const auto money  = reinterpret_cast<uint64_t&>(stats.Money);
				    const auto wallet = reinterpret_cast<uint64_t&>(stats.WalletBalance);


				    ImGui::BeginGroup();
				    {
					    if (boss_goon.Language >= 0 && boss_goon.Language < 13)
						    ImGui::Text("Language: %s", languages[boss_goon.Language].name);
					    ImGui::Text("Money In Wallet: %llu", wallet);
					    ImGui::Text("Money In Bank: %llu", money - wallet);
					    ImGui::Text("Total Money: %llu", money);
					    ImGui::Text("Rank: %d (RP %d)", stats.Rank, stats.RP);
					    ImGui::Text("K/D Ratio: %f / Kills: %d / Deaths: %d", stats.KdRatio, stats.KillsOnPlayers, stats.DeathsByPlayers);
					    ImGui::Text("Rockstar Id: %d", rockstar_id);
					    ImGui::Text("Male: %d", globalplayer_bd.IsMale);
					    ImGui::Text("Bad sport: %d", globalplayer_bd.IsBadsport);
					    ImGui::Text("Rockstar dev (dev dlc check, yim): %d", globalplayer_bd.IsRockstarDev);

					    ImGui::Spacing();
					    ImGui::Text("NAT Type: %s", get_nat_type_str(g_player_service->get_selected()->get_net_data()->m_nat_type));

					    auto ip = (current_player == g_player_service->get_self()) ? current_player->get_net_data()->m_external_ip :
					                                                                 current_player->get_ip_address();
					    auto port = (current_player == g_player_service->get_self()) ? current_player->get_net_data()->m_external_port :
					                                                                   current_player->get_port();

					    if (ip)
					    {
						    ImGui::Text("IP Address: %d.%d.%d.%d:%d",
						        ip.value().m_field1,
						        ip.value().m_field2,
						        ip.value().m_field3,
						        ip.value().m_field4,
						        port);
						    ImGui::SameLine();
						    if (ImGui::SmallButton("copy##copyip"))
							    ImGui::SetClipboardText(std::format("{}{}.{}.{}.{}:{}",
							        ip_viewer_link,
							        ip.value().m_field1,
							        ip.value().m_field2,
							        ip.value().m_field3,
							        ip.value().m_field4,
							        port)
							                                .c_str());
					    }
					    else if (auto net_player_data = current_player->get_net_data())
					    {
						    ImGui::Text(net_player_data->m_force_relays ? "IP Address: Force Relay" : "IP Address: Unknown");

						    auto conn_peer = current_player->get_connection_peer();
						    auto cxn_type  = conn_peer ? conn_peer->m_peer_address.m_connection_type : 0;
						    netAddress ip;

						    if (cxn_type == 2)
						    {
							    ip = conn_peer->m_relay_address.m_relay_address;
							    ImGui::Text("Relay IP Address: %d.%d.%d.%d", ip.m_field1, ip.m_field2, ip.m_field3, ip.m_field4);
						    }
						    else if (cxn_type == 3)
						    {
							    ip = conn_peer->m_peer_address.m_relay_address;
							    ImGui::Text("Peer Relay IP : %d.%d.%d.%d", ip.m_field1, ip.m_field2, ip.m_field3, ip.m_field4);
						    }
						    if (cxn_type == 2 || cxn_type == 3)
						    {
							    ImGui::SameLine();
							    if (ImGui::SmallButton("copy##copyip"))
								    ImGui::SetClipboardText(std::format("{}{}.{}.{}.{}", ip_viewer_link, ip.m_field1, ip.m_field2, ip.m_field3, ip.m_field4)
								                                .c_str());
						    }
					    }

					    if (auto net_data = current_player->get_net_data())
					    {
						    ImGui::Text("Host token: %d", net_data->m_host_token);
						    ImGui::SameLine();
						    if (ImGui::SmallButton("copy##copyHtoken"))
							    ImGui::SetClipboardText(std::format("{}", net_data->m_host_token).c_str());
					    }
				    }
				    ImGui::EndGroup();
				    ImGui::SameLine();
				    ImGui::BeginGroup();
				    {
					    ImGui::Text("Freemode State: %s", get_freemode_state(globalplayer_bd.FreemodeState));
					    ImGui::Text("Session Join State: %s", get_player_state(globalplayer_bd.PlayerStateFlags));

					    ImGui::Spacing();

					    ImGui::Text("Is in interior: %d", globalplayer_bd.CurrentInteriorIndex != 0);
					    // ImGui::Text("In mission: %d", globalplayer_bd.MissionType != eMissionType::NONE);
					    ImGui::Text("Off radar: %d", globalplayer_bd.OffRadarActive);
					    ImGui::Text("Is invisible: %d", globalplayer_bd.IsInvisible);
					    if (current_player->last_killed_by && current_player->last_killed_by->is_valid())
						    ImGui::Text("Last killed by: %s", current_player->last_killed_by->get_name());

					    ImGui::Spacing();

					    std::string mode_str;

					    if (CPed* ped = current_player->get_ped(); ped != nullptr)
					    {
						    if (ped->m_damage_bits & (uint32_t)eEntityProofs::GOD)
							    ImGui::Text("Player God Mod");
						    else if (ped->m_damage_bits & (uint32_t)eEntityProofs::BULLET)
							    ImGui::Text("Player Bullet Proof");
						    else if (ped->m_damage_bits & (uint32_t)eEntityProofs::EXPLOSION)
							    ImGui::Text("Player Explosion Proof");

						    if (ped->m_ped_task_flag & (uint8_t)ePedTask::TASK_DRIVING)
							    if (auto vehicle = current_player->get_current_vehicle(); vehicle != nullptr)
							    {
								    if (CVehicleModelInfo* vehicle_model_info = static_cast<CVehicleModelInfo*>(vehicle->m_model_info))
									    ImGui::Text("Vehicle: %s",
									        vehicle::get_vehicle_model_name(
									            g_gta_data_service->vehicles()[vehicle_model_info->m_hash])
									            .c_str());

								    if (vehicle->m_damage_bits & (uint32_t)eEntityProofs::GOD)
									    ImGui::Text("Vehicle God Mod");
							    }
					    }
				    }
				    ImGui::EndGroup();

				    ImGui::Separator();

				    ImGui::Checkbox("Block Explosions", &current_player->block_explosions);
				    ImGui::Checkbox("Block Clone Creates", &current_player->block_clone_create);
				    ImGui::Checkbox("Block Clone Syncs", &current_player->block_clone_sync);
				    ImGui::Checkbox("Block Network Events", &current_player->block_net_events);
				    ImGui::Checkbox("Log Clones", &current_player->log_clones);
			    }
		    },
		    false);
	}

	static inline void render_info(player_ptr current_player)
	{
		ImGui::BeginGroup();
		{
			components::sub_title("Info");

			extra_info_button(current_player);
			ImGui::SameLine();
			if (components::button("Copy Name & SC id##copyname"))
				ImGui::SetClipboardText(std::format("{} {}", current_player->get_name(), rockstar_id).c_str());

			ImGui::Spacing();

			components::button(current_player->is_blocked ? "Un-Block" : "Block", [current_player] {
				current_player->is_blocked = !current_player->is_blocked;

				if (bad_players_nm::does_exist(rockstar_id))
					bad_players_nm::toggle_block(rockstar_id, current_player->is_blocked);
				else if (current_player->is_blocked)
					bad_players_nm::add_player({current_player->get_name(), rockstar_id, true, current_player->is_spammer});

				if (current_player->is_blocked && g_player_service->get_self()->is_host())
					dynamic_cast<player_command*>(command::get(RAGE_JOAAT("hostkick")))->call(current_player);
			});
			ImGui::SameLine();
			if (components::button(current_player->is_modder ? "Un-flag Modder" : "Flag Modder"))
				current_player->is_modder = !current_player->is_modder;
			ImGui::SameLine();
			components::button(current_player->is_known_player ? "Unmark Known" : "Mark Known", [current_player] {
				known_player_nm::toggle(current_player);
			});
		}
		ImGui::EndGroup();
	}

	static inline void render_chat_spam(player_ptr current_player)
	{
		ImGui::BeginGroup();
		{
			components::sub_title("Chat Spammer");
			if (current_player->is_spammer)
				if (ImGui::BeginListBox("##message", ImVec2(350, 100)))
				{
					ImGui::TextWrapped(current_player->spam_message.c_str());
					ImGui::EndListBox();
				}

			components::button(current_player->is_spammer ? "Un-flag Spammer" : "Flag Spammer", [current_player] {
				current_player->is_spammer = !current_player->is_spammer;

				if (bad_players_nm::does_exist(rockstar_id))
					bad_players_nm::set_spammer(rockstar_id, current_player->is_spammer);
				else if (current_player->is_spammer)
					bad_players_nm::add_player({current_player->get_name(), rockstar_id, false, true});
			});
		}
		ImGui::EndGroup();
	}

	static inline void render_tel_loc(player_ptr current_player)
	{
		ImGui::BeginGroup();
		{
			auto& globalplayer_bd = scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[current_player->id()];
			bool highlight_player = globalplayer_bd.PlayerBlip.BlipFlags.IsSet(eBlipFlags::kFlashBlip)
			    && globalplayer_bd.PlayerBlip.BlipFlags.IsSet(eBlipFlags::kFlashMinimapDisplay);

			components::sub_title("Teleport / Location");

			components::button("Set Waypoint", [current_player] {
				if (globals::get_interior_from_player(current_player->id()) != 0)
				{
					auto blips = g_pointers->m_gta.m_blip_list->m_Blips;
					for (int i = 0; i < 1500; i++)
						if (auto blip = blips[i].m_pBlip;
						    blip && blip->m_message && !strcmp(blip->m_message, current_player->get_name()))
							return HUD::SET_NEW_WAYPOINT(blip->m_x, blip->m_y);

					return g_notification_service->push_error("Failed", "Player in interior. Try open map and try again.");
				}

				Vector3 location = ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(current_player->id()), 0);
				HUD::SET_NEW_WAYPOINT(location.x, location.y);
			});
			ImGui::SameLine();
			if (ImGui::Checkbox("Highlight", &highlight_player))
			{
				if (highlight_player)
				{
					globalplayer_bd.PlayerBlip.BlipFlags.Set(eBlipFlags::kFlashBlip);
					globalplayer_bd.PlayerBlip.BlipFlags.Set(eBlipFlags::kFlashMinimapDisplay);
				}
				else
				{
					globalplayer_bd.PlayerBlip.BlipFlags.Clear(eBlipFlags::kFlashBlip);
					globalplayer_bd.PlayerBlip.BlipFlags.Clear(eBlipFlags::kFlashMinimapDisplay);
				}
			}

			ImGui::Spacing();

			components::button("TP", [current_player] {
				Vector3 location = ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(current_player->id()), 0);
				teleport::to_coords(location);
			});
			ImGui::SameLine();
			components::button("TP IN VEH", [current_player] {
				if (Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(current_player->id()), 0); veh)
					teleport::into_vehicle(veh);
			});
		}
		ImGui::EndGroup();
	}

	static inline void render_misc(player_ptr current_player)
	{
		ImGui::BeginGroup();
		{
			components::sub_title("Misc");

			components::button("Copy outfit", [current_player] {
				steal_player_outfit(current_player);
			});
			ImGui::SameLine();
			components::button("Save outfit", [current_player] {
				steal_player_outfit(current_player, true);
			});

			components::ver_space();

			components::button("Copy Vehicle", [current_player] {
				if (Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(current_player->id()), 0); veh)
					persist_car_service::clone_ped_car(veh, current_player->get_name());
				else
					g_notification_service->push_error("Copy Vehicle", "Failed to get veh", false);
			});
			ImGui::SameLine();
			components::button("Save Vehicle", [current_player] {
				if (Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(current_player->id()), 0); veh)
					persist_car_service::save_vehicle(veh, "", "");
				else
					g_notification_service->push_error("Save Vehicle", "Failed to get veh", false);
			});

			components::ver_space();

			components::button("Free aiming?", [current_player] {
				g_notification_service->push(PLAYER::IS_PLAYER_FREE_AIMING(current_player->id()) ? "Free aiming" : "Aimbot", "");
			});
		}
		ImGui::EndGroup();
	}

	static inline void render_toxic(player_ptr current_player)
	{
		ImGui::BeginGroup();
		if (!current_player->is_host() || *g_pointers->m_gta.m_is_session_started) // to prevent you from kicking host when session has not started.
		{
			components::sub_title("Kick");

			if (g_player_service->get_self()->is_host())
				components::player_command_button<"hostkick">(current_player);

			ImGui::Spacing();

			// components::player_command_button<"shkick">(current_player);
			components::player_command_button<"endkick">(current_player);
			components::player_command_button<"nfkick">(current_player);
			components::player_command_button<"oomkick">(current_player);

			ImGui::Spacing();

			if (!current_player->is_host())
				components::player_command_button<"desync">(current_player);

			components::ver_space();

			components::sub_title("Other");

			components::button("Timeout", [current_player] {
				current_player->timeout();
			});
			ImGui::SameLine();
			components::button("Un-timeout", [current_player] {
				current_player->timeout(false);
			});
		}
		ImGui::EndGroup();
	}

	static inline void render_infractions(player_ptr current_player)
	{
		if (!current_player->infractions.empty())
		{
			ImGui::BeginGroup();
			components::sub_title("Infractions");
			components::button("Clear infractions", [current_player] {
				current_player->infractions.clear();
			});
			for (auto infraction : current_player->infractions)
				ImGui::BulletText(std::format("{} - {}", infraction.first->m_notify_message, infraction.second).c_str());
			ImGui::EndGroup();
		}
	}

	void view::view_player()
	{
		ImGui::Spacing();

		if (g_player_service->get_selected()->is_valid())
		{
			player_ptr current_player = g_player_service->get_selected();

			if (last_selected_player != current_player)
			{
				last_selected_player          = current_player;
				navigation_struct& player_tab = g_gui_service->get_navigation().at(tabs::PLAYER);

				strcpy(player_tab.name, current_player->id() == self::id ? "you" : current_player->get_name());
				strcat(player_tab.name, std::format(" ({})", std::to_string(current_player->id())).data());

				if (current_player->is_host())
					strcat(player_tab.name, " [HOST]");
				if (current_player->is_friend())
					strcat(player_tab.name, " [FRIEND]");
				if (current_player->is_modder)
					strcat(player_tab.name, " [MOD]");
				if (current_player->is_blocked)
					strcat(player_tab.name, " [BLOCKED]");
				if (current_player->is_known_player)
					strcat(player_tab.name, " [KNOWN]");

				if (auto net_player_data = current_player->get_net_data())
					rockstar_id = net_player_data->m_gamer_handle.m_rockstar_id;
			}

			if (current_player->id() == self::id)
			{
				extra_info_button(current_player);
				return;
			}

			ImGui::BeginGroup();
			{
				render_info(current_player);

				components::ver_space();

				render_chat_spam(current_player);

				components::ver_space();

				render_infractions(current_player);
			}
			ImGui::EndGroup();
			ImGui::SameLine(0, 50);
			ImGui::BeginGroup();
			{
				render_tel_loc(current_player);

				components::ver_space();

				render_misc(current_player);
			}
			ImGui::EndGroup();
			ImGui::SameLine(0, 50);
			ImGui::BeginGroup();
			{
				render_toxic(current_player);
			}
			ImGui::EndGroup();
		}
	}
}