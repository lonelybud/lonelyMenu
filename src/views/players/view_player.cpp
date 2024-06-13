#include "core/data/language_codes.hpp"
#include "core/data/lua_scripts.hpp"
#include "core/data/protections.hpp"
#include "core/data/region_codes.hpp"
#include "core/data/self.hpp"
#include "core/scr_globals.hpp"
#include "gta_util.hpp"
#include "gui.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "services/bad_players/bad_players.hpp"
#include "services/gta_data/gta_data_service.hpp"
#include "services/gui/gui_service.hpp"
#include "services/known_players/known_players.hpp"
#include "services/notifications/notification_service.hpp"
#include "services/vehicle/persist_car_service.hpp"
#include "util/chat.hpp"
#include "util/delete_entity.hpp"
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
	static constexpr char priv[]           = "[PRIVATE]: ";
	static constexpr char ip_viewer_link[] = "https://iplogger.org/ip-tracker/?ip=";
	static big::player_ptr last_selected_player;
	static rock_id rockstar_id;
	static bool open_gift_veh_model = false;

	static inline const char* get_nat_type_str(int type)
	{
		switch (type)
		{
		case 0:
		case 1:
		case 2:
		case 3: return nat_types[type].name;
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

	static void toggle_block(bool v)
	{
		if (!g_bad_players_service.does_exist(rockstar_id))
			g_bad_players_service.add_player(last_selected_player, false, last_selected_player->is_spammer);

		if (v && !g_bad_players_service.is_blocked(rockstar_id))
			g_bad_players_service.toggle_block(rockstar_id, true);

		if (!v && g_bad_players_service.is_blocked(rockstar_id))
			g_bad_players_service.toggle_block(rockstar_id, false);

		if (v && g_player_service->get_self()->is_host())
			dynamic_cast<player_command*>(command::get("breakup"_J))->call(last_selected_player);
	}

	static void extra_info_button()
	{
		components::options_modal(
		    "Extra Info",
		    [] {
			    if (auto id = last_selected_player->id(); id != -1)
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
					    ImGui::Text("NAT Type: %s", get_nat_type_str(last_selected_player->get_net_data()->m_nat_type));

					    auto ip   = last_selected_player == g_player_service->get_self() ?
					          last_selected_player->get_net_data()->m_external_ip :
					          last_selected_player->get_ip_address();
					    auto port = last_selected_player == g_player_service->get_self() ?
					        last_selected_player->get_net_data()->m_external_port :
					        last_selected_player->get_port();

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
							    ImGui::SetClipboardText(std::format("{}{}.{}.{}.{}",
							        ip_viewer_link,
							        ip.value().m_field1,
							        ip.value().m_field2,
							        ip.value().m_field3,
							        ip.value().m_field4)
							                                .c_str());
					    }
					    else if (auto net_player_data = last_selected_player->get_net_data())
					    {
						    ImGui::Text(net_player_data->m_force_relays ? "IP Address: Force Relay" : "IP Address: Unknown");

						    auto conn_peer = last_selected_player->get_connection_peer();
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

					    if (last_selected_player != g_player_service->get_self())
					    {
						    ImGui::Text("Host token: %llu", last_selected_player->m_host_token);
						    ImGui::SameLine();
						    if (ImGui::SmallButton("copy##copyHtoken"))
							    ImGui::SetClipboardText(std::format("{}", last_selected_player->m_host_token).c_str());
						    if (last_selected_player->m_host_token < g_session.host_token)
							    ImGui::Text("Host token is smaller than you");
					    }
				    }
				    ImGui::EndGroup();
				    ImGui::SameLine();
				    ImGui::BeginGroup();
				    {
					    ImGui::Text("Freemode State: %s", get_freemode_state(globalplayer_bd.FreemodeState));
					    ImGui::Text("Session Join State: %s", get_player_state(globalplayer_bd.PlayerStateFlags));

					    ImGui::Spacing();

					    ImGui::Text("Interior: %d", get_interior_from_player(id));
					    // ImGui::Text("In mission: %d", globalplayer_bd.MissionType != eMissionType::NONE);
					    ImGui::Text("Off radar: %d", globalplayer_bd.OffRadarActive);
					    ImGui::Text("Is invisible: %d", globalplayer_bd.IsInvisible);
					    ImGui::Text("Is invisible (Player list): %d", is_hidden_from_player_list(last_selected_player));
					    if (last_selected_player->last_killed_by && last_selected_player->last_killed_by->is_valid())
						    ImGui::Text("Last killed by: %s", last_selected_player->last_killed_by->m_name);

					    ImGui::Spacing();

					    std::string mode_str;

					    if (CPed* ped = last_selected_player->get_ped(); ped != nullptr)
					    {
						    if (ped->m_damage_bits & (uint32_t)eEntityProofs::GOD)
							    ImGui::Text("Player God Mod");
						    else if (ped->m_damage_bits & (uint32_t)eEntityProofs::BULLET)
							    ImGui::Text("Player Bullet Proof");
						    else if (ped->m_damage_bits & (uint32_t)eEntityProofs::EXPLOSION)
							    ImGui::Text("Player Explosion Proof");

						    if (ped->m_ped_task_flag & (uint8_t)ePedTask::TASK_DRIVING)
							    if (auto vehicle = last_selected_player->get_current_vehicle(); vehicle != nullptr)
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

				    if (last_selected_player != g_player_service->get_self())
				    {
					    ImGui::Checkbox("Block Explosions", &last_selected_player->block_explosions);
					    ImGui::Checkbox("Block Clone Creates", &last_selected_player->block_clone_create);
					    ImGui::Checkbox("Block Clone Syncs", &last_selected_player->block_clone_sync);
					    ImGui::Checkbox("Block Network Events", &last_selected_player->block_net_events);

					    ImGui::Spacing();

					    ImGui::Checkbox("Block Radio Requests", &last_selected_player->block_radio_requests);
					    ImGui::Checkbox("Block Host Migration Requests", &last_selected_player->block_host_migr_requests);

					    ImGui::Spacing();

					    ImGui::Checkbox("Log Clones", &last_selected_player->log_clones);
				    }
			    }
		    },
		    false,
		    "Info##infobtn");
	}

	static inline void render_info()
	{
		ImGui::BeginGroup();
		{
			components::sub_title("Info");

			components::command_checkbox<"spectate">();
			ImGui::Spacing();
			extra_info_button();
			ImGui::SameLine();
			if (components::button("Copy Name & SC id##copyname"))
				ImGui::SetClipboardText(std::format("{} {}", last_selected_player->m_name, rockstar_id).c_str());
			ImGui::Spacing();
			components::button("Log connection stats", [] {
				auto id = last_selected_player->id();
				LOGF(VERBOSE, "Relay: {}, Latency: {}, Ping: {}, Packet Loss: {}", NETWORK::NETWORK_IS_CONNECTED_VIA_RELAY(id), NETWORK::NETWORK_GET_AVERAGE_LATENCY(id), NETWORK::NETWORK_GET_AVERAGE_PING(id), NETWORK::NETWORK_GET_AVERAGE_PACKET_LOSS(id));
			});
		}
		ImGui::EndGroup();
	}

	static inline void render_flags()
	{
		ImGui::BeginGroup();
		{
			components::sub_title("Flags");

			ImGui::BeginGroup();
			{
				ImGui::Checkbox("Esp Enemy", &last_selected_player->esp_enemy);

				if (ImGui::Checkbox("Is Blocked", &last_selected_player->is_blocked))
					toggle_block(last_selected_player->is_blocked);

				if (!last_selected_player->is_blocked && ImGui::Checkbox("Is Modder", &last_selected_player->is_modder))
					g_bad_players_service.add_player(last_selected_player, false, last_selected_player->is_spammer);

				if (ImGui::Checkbox("Is Known", &last_selected_player->is_known_player))
					last_selected_player->is_known_player ? g_known_players_service.add(last_selected_player) :
					                                        g_known_players_service.remove(last_selected_player);

				ImGui::Checkbox("Whitelist Spammer", &last_selected_player->whitelist_spammer);

				ImGui::Checkbox("Whitelist Ptfx", &last_selected_player->whitelist_ptfx);

				if (ImGui::Checkbox("Timeout", &last_selected_player->timed_out))
					g_fiber_pool->queue_job([] {
						last_selected_player->timeout();
					});

				ImGui::Checkbox("Is Other", &last_selected_player->is_other);
			}
			ImGui::EndGroup();
			ImGui::SameLine();
			ImGui::BeginGroup();
			{
				if (ImGui::BeginListBox("##message", ImVec2(300, 100)))
				{
					ImGui::TextWrapped(last_selected_player->spam_message.c_str());
					ImGui::EndListBox();
				}

				if (g_known_players_service.is_known(rockstar_id))
				{
					ImGui::Text("Known Player Message - ");
					if (ImGui::BeginListBox("##known_message", ImVec2(300, 100)))
					{
						ImGui::TextWrapped(g_known_players_service.known_players_list[rockstar_id].m.c_str());
						ImGui::EndListBox();
					}
				}
			}
			ImGui::EndGroup();
		}
		ImGui::EndGroup();
	}

	static inline void render_tel_loc()
	{
		ImGui::BeginGroup();
		{
			auto& globalplayer_bd = scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[last_selected_player->id()];
			bool highlight_player = globalplayer_bd.PlayerBlip.BlipFlags.IsSet(eBlipFlags::kFlashBlip)
			    && globalplayer_bd.PlayerBlip.BlipFlags.IsSet(eBlipFlags::kFlashMinimapDisplay);

			components::sub_title("Teleport / Location");

			components::button("Set Waypoint", [] {
				if (get_interior_from_player(last_selected_player->id()) != 0)
				{
					auto blips = g_pointers->m_gta.m_blip_list->m_Blips;
					for (int i = 0; i < 1500; i++)
						if (auto blip = blips[i].m_pBlip;
						    blip && blip->m_message && !strcmp(blip->m_message, last_selected_player->m_name))
							return HUD::SET_NEW_WAYPOINT(blip->m_x, blip->m_y);

					return g_notification_service.push_error("Failed", "Player in interior. Try open map and try again.");
				}
				if (auto ped = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(last_selected_player->id()))
				{
					Vector3 location = ENTITY::GET_ENTITY_COORDS(ped, 0);
					HUD::SET_NEW_WAYPOINT(location.x, location.y);
				}
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

			components::button("TP", [] {
				if (auto ped = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(last_selected_player->id()))
					teleport::to_coords(ENTITY::GET_ENTITY_COORDS(ped, 0));
			});
			ImGui::SameLine();
			components::button("TP IN VEH", [] {
				if (auto ped = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(last_selected_player->id()))
					if (Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(ped, 0); veh)
						teleport::into_vehicle(veh);
			});
		}
		ImGui::EndGroup();
	}

	static inline void render_good_options()
	{
		ImGui::BeginGroup();
		{
			components::sub_title("Good Options");

			components::button("Set Veh God", [] {
				if (auto ped = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(last_selected_player->id()))
					if (Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(ped, 0); veh && entity::take_control_of(veh))
					{
						ENTITY::SET_ENTITY_INVINCIBLE(veh, TRUE);
						g_notification_service.push_success("Success", "Entity is invincible now");
						return;
					}
				g_notification_service.push_error("Failed", "To make invincible.");
			});
			ImGui::SameLine();
			components::button("Unset Veh God", [] {
				if (auto ped = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(last_selected_player->id()))
					if (Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(ped, 0); veh && entity::take_control_of(veh))
					{
						ENTITY::SET_ENTITY_INVINCIBLE(veh, FALSE);
						g_notification_service.push_success("Failed", "Entity is not invincible now");
						return;
					}
				g_notification_service.push_success("Failed", "To undo invincibility");
			});

			components::ver_space();

			components::button("Give Persist Veh", [] {
				if (auto ped = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(last_selected_player->id()))
				{
					g_vehicle.spawn_inside = false;
					persist_car_service::load_vehicle(std::nullopt, ped);
				}
			});

			components::ver_space();

			if (components::button("** Gift Veh **"))
				open_gift_veh_model = true;
		}
		ImGui::EndGroup();
	}

	static inline void render_misc()
	{
		ImGui::BeginGroup();
		{
			components::sub_title("Misc");

			components::button("Copy outfit", [] {
				steal_player_outfit(last_selected_player);
			});
			ImGui::SameLine();
			components::button("Save outfit", [] {
				steal_player_outfit(last_selected_player, true);
			});

			components::ver_space();

			components::button("Copy Vehicle", [] {
				if (auto ped = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(last_selected_player->id()))
				{
					if (Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(ped, 0); veh)
						persist_car_service::clone_ped_car(veh, last_selected_player->m_name);
					else
						g_notification_service.push_error("Copy Vehicle", "Failed to get veh", false);
				}
			});
			ImGui::SameLine();
			components::button("Save Vehicle", [] {
				if (auto ped = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(last_selected_player->id()))
				{
					if (Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(ped, 0); veh)
						persist_car_service::save_vehicle(veh, "", "");
					else
						g_notification_service.push_error("Save Vehicle", "Failed to get veh", false);
				}
			});
		}
		ImGui::EndGroup();
	}

	static inline void render_toxic()
	{
		ImGui::BeginGroup();
		if (!last_selected_player->is_host() || *g_pointers->m_gta.m_is_session_started) // to prevent you from kicking host when session has not started.
		{
			components::sub_title("Kick");

			if (g_player_service->get_self()->is_host())
			{
				if (components::button("Block Kick"))
					toggle_block(true);
				components::player_command_button<"breakup">(last_selected_player);
				components::player_command_button<"hostkick">(last_selected_player);
			}

			ImGui::Spacing();

			components::player_command_button<"oomkick">(last_selected_player);

			ImGui::Spacing();

			// components::player_command_button<"shkick">(last_selected_player);
			components::player_command_button<"desync">(last_selected_player);

			components::ver_space();

			components::sub_title("Other");

			components::player_command_button<"vehkick">(last_selected_player);

			components::button("Delete Vehicle", [] {
				if (auto c_veh = last_selected_player->get_current_vehicle())
				{
					auto veh = g_pointers->m_gta.m_ptr_to_handle(c_veh);
					entity::delete_entity(veh);
				}
			});

			components::player_command_button<"intkick">(last_selected_player);
		}
		ImGui::EndGroup();
	}

	static inline void render_infractions()
	{
		if (!last_selected_player->infractions.empty())
		{
			ImGui::BeginGroup();
			components::sub_title("Infractions");
			components::button("Clear infractions", [] {
				last_selected_player->infractions.clear();
				last_selected_player->is_pain_in_ass = false;
			});
			for (auto infraction : last_selected_player->infractions)
				ImGui::BulletText(std::format("{} - {}", infraction.first->m_event_name, infraction.second).c_str());
			ImGui::EndGroup();
		}
	}

	// static inline void render_chat()
	// {
	// 	ImGui::BeginGroup();
	// 	components::sub_title("Chat");

	// 	static char temp[256];
	// 	static char msg[256 - sizeof(priv)];

	// 	ImGui::SetNextItemWidth(200);
	// 	components::input_text_with_hint("###chatmessage", "Message", msg, sizeof(msg));
	// 	if (components::button("Send Message"))
	// 		g_fiber_pool->queue_job([] {
	// 			strcpy(temp, priv);
	// 			strcat(temp, msg);
	// 			chat::send_message(temp, last_selected_player, false, true);
	// 		});
	// 	ImGui::EndGroup();
	// }

	void view::view_player()
	{
		ImGui::Spacing();

		auto current_player = g_player_service->get_selected();

		if (current_player && current_player->is_valid())
		{
			if (last_selected_player != current_player)
			{
				last_selected_player          = current_player;
				navigation_struct& player_tab = g_gui_service->get_navigation().at(tabs::PLAYER);

				strcpy(player_tab.name, last_selected_player->id() == self::id ? "you" : last_selected_player->m_name);
				strcat(player_tab.name, std::format(" ({})", std::to_string(last_selected_player->id())).data());

				if (last_selected_player->is_host())
					strcat(player_tab.name, " [HOST]");
				if (last_selected_player->is_friend())
					strcat(player_tab.name, " [FRIEND]");
				if (last_selected_player->is_modder)
					strcat(player_tab.name, " [MOD]");
				if (last_selected_player->is_blocked)
					strcat(player_tab.name, " [BLOCKED]");
				if (last_selected_player->is_known_player)
					strcat(player_tab.name, " [KNOWN]");

				rockstar_id = last_selected_player->m_rockstar_id;
			}

			if (last_selected_player->id() == self::id)
			{
				extra_info_button();
				return;
			}

			ImGui::BeginGroup();
			{
				render_info();

				components::ver_space();

				render_flags();

				components::ver_space();

				render_infractions();
			}
			ImGui::EndGroup();
			ImGui::SameLine(0, 50);
			ImGui::BeginGroup();
			{
				render_tel_loc();

				components::ver_space();

				render_misc();

				components::ver_space();

				render_good_options();
			}
			ImGui::EndGroup();
			ImGui::SameLine(0, 50);
			ImGui::BeginGroup();
			{
				render_toxic();

				// components::ver_space();
				// render_chat();
			}
			ImGui::EndGroup();

			if (open_gift_veh_model)
				ImGui::OpenPopup("##gift_veh_model");
			if (ImGui::BeginPopupModal("##gift_veh_model", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove))
			{
				ImGui::Text("Are you sure you want to gift veh");
				ImGui::Spacing();
				if (ImGui::Button("Yes"))
				{
					g_fiber_pool->queue_job([] {
						lua_scripts::gift_veh(last_selected_player);
					});

					open_gift_veh_model = false;
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("No"))
				{
					open_gift_veh_model = false;
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
		}
	}
}