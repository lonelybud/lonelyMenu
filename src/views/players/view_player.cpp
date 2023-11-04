#include "core/data/language_codes.hpp"
#include "core/data/player.hpp"
#include "core/scr_globals.hpp"
#include "core/settings/protections.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "services/gui/gui_service.hpp"
#include "services/recent_modders/recent_modders.hpp"
#include "services/vehicle/persist_car_service.hpp"
#include "util/delete_entity.hpp"
#include "util/globals.hpp"
#include "util/scripts.hpp"
#include "views/view.hpp"

#include <network/netConnection.hpp>
#include <script/globals/GPBD_FM.hpp>
#include <script/globals/GPBD_FM_3.hpp>
#include <script/globals/GlobalPlayerBD.hpp>

inline void ver_Space()
{
	ImGui::Dummy(ImVec2(0.0f, ImGui::GetTextLineHeight()));
}

namespace big
{
	static inline big::player_ptr last_selected_player;
	static uint64_t rockstar_id;

	static void extra_info_button(player_ptr current_player)
	{
		components::options_modal(
		    "Extra Info",
		    [current_player] {
			    ImGui::BeginGroup();
			    if (auto id = current_player->id(); id != -1)
			    {
				    auto& stats     = scr_globals::gpbd_fm_1.as<GPBD_FM*>()->Entries[id].PlayerStats;
				    auto& boss_goon = scr_globals::gpbd_fm_3.as<GPBD_FM_3*>()->Entries[id].BossGoon;

				    const auto money  = reinterpret_cast<uint64_t&>(stats.Money);
				    const auto wallet = reinterpret_cast<uint64_t&>(stats.WalletBalance);

				    if (boss_goon.Language >= 0 && boss_goon.Language < 13)
					    ImGui::Text("Language: %s", languages[boss_goon.Language].name);

				    ImGui::Text("Money In Wallet: %llu", wallet);
				    ImGui::Text("Money In Bank: %llu", money - wallet);
				    ImGui::Text("Total Money: %llu", money);
				    ImGui::Text("Rank: %d (RP %d)", stats.Rank, stats.RP);
				    ImGui::Text("K/D Ratio: %f", stats.KdRatio);
				    ImGui::Text("Kills On Players: %d", stats.KillsOnPlayers);
				    ImGui::Text("Deaths By Players: %d", stats.DeathsByPlayers);

				    ImGui::Spacing();
				    auto ip   = current_player->get_ip_address();
				    auto port = current_player->get_port();

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
						    ImGui::SetClipboardText(std::format("{}.{}.{}.{}:{}",
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

					    if (g_protections.force_relay_connections)
						    ImGui::Text("Note - IP addresses cannot be seen when Force Relay Connections is enabled.");
					    else
					    {
						    auto conn_peer = current_player->get_connection_peer();
						    auto cxn_type  = conn_peer ? conn_peer->m_peer_address.m_connection_type : 0;
						    netAddress ip;

						    if (cxn_type == 2)
						    {
							    ip = conn_peer->m_relay_address.m_relay_address;
							    ImGui::Text(std::format("Relay IP Address: {}.{}.{}.{}", ip.m_field1, ip.m_field2, ip.m_field3, ip.m_field4)
							                    .c_str());
						    }
						    else if (cxn_type == 3)
						    {
							    ip = conn_peer->m_peer_address.m_relay_address;
							    ImGui::Text(std::format("Peer Relay IP : {}.{}.{}.{}", ip.m_field1, ip.m_field2, ip.m_field3, ip.m_field4)
							                    .c_str());
						    }
						    if (cxn_type == 2 || cxn_type == 3)
							    if (ImGui::SmallButton("copy##copyip"))
								    ImGui::SetClipboardText(
								        std::format("{}.{}.{}.{}", ip.m_field1, ip.m_field2, ip.m_field3, ip.m_field4).c_str());
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
		    },
		    false);
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

				strcpy(player_tab.name, current_player->get_name());
				strcat(player_tab.name, std::format(" ({})", std::to_string(current_player->id())).data());

				if (current_player->is_host())
					strcat(player_tab.name, " [HOST]");
				if (current_player->is_friend())
					strcat(player_tab.name, " [FRIEND]");
				if (current_player->is_modder)
					strcat(player_tab.name, " [MOD]");

				if (auto net_data = current_player->get_net_data())
				{
					rockstar_id = net_data->m_gamer_handle.m_rockstar_id;
					if (recent_modders_nm::is_blocked(rockstar_id))
						strcat(player_tab.name, " [BLOCKED]");
				}
			}

			if (current_player->id() == self::id)
			{
				extra_info_button(current_player);
				return;
			}

			ImGui::BeginGroup();
			{
				ImGui::BeginGroup();
				{
					ImGui::Checkbox("Spectate", &g_player.spectating);
					ImGui::SameLine();
					if (current_player->is_host())
					{
						auto auto_kick_host =
						    g_player.host_to_auto_kick != nullptr && g_player.host_to_auto_kick->id() == current_player->id();
						if (ImGui::Checkbox("Kick host when attacked", &auto_kick_host))
							g_player.host_to_auto_kick = auto_kick_host ? current_player : nullptr;
					}

					ImGui::Spacing();
					components::sub_title("Info");
					extra_info_button(current_player);
					ImGui::SameLine();
					components::button("SC Profile", [current_player] {
						uint64_t gamerHandle[13];
						NETWORK::NETWORK_HANDLE_FROM_PLAYER(current_player->id(), (Any*)&gamerHandle, 13);
						NETWORK::NETWORK_SHOW_PROFILE_UI((Any*)&gamerHandle);
					});
					ImGui::SameLine();
					if (components::button("Copy Name##copyname"))
						ImGui::SetClipboardText(current_player->get_name());
					ImGui::SameLine(0, 2.0f * ImGui::GetTextLineHeight());
					if (bool is_blocked = recent_modders_nm::is_blocked(rockstar_id); components::button(is_blocked ? "Unblock Join" : "Block Join"))
					{
						if (recent_modders_nm::does_exist(rockstar_id))
							recent_modders_nm::toggle_block(rockstar_id);
						else
						{
							recent_modders_nm::add_player({current_player->get_name(), rockstar_id, true});

							if (g_player_service->get_self()->is_host())
								dynamic_cast<player_command*>(command::get(RAGE_JOAAT("hostkick")))->call(current_player, {});
						}
					}
				}
				ImGui::EndGroup();
				if (current_player->is_spammer)
				{
					ver_Space();
					ImGui::BeginGroup();
					{
						components::sub_title("Chat Spammer");
						if (ImGui::BeginListBox("##message", ImVec2(350, 100)))
						{
							ImGui::TextWrapped(current_player->spam_message.c_str());
							ImGui::EndListBox();
						}
						if (components::button("Unflag Spammer"))
							current_player->is_spammer = false;
					}
					ImGui::EndGroup();
				}
				ver_Space();
				ImGui::BeginGroup();
				{
					components::sub_title("Misc");

					components::player_command_button<"clearwanted">(current_player);
					ImGui::Spacing();
					components::player_command_button<"givehealth">(current_player);
					ImGui::SameLine();
					components::player_command_button<"givearmor">(current_player);
					ImGui::SameLine();
					components::player_command_button<"giveammo">(current_player);

					ver_Space();
					components::player_command_button<"copyoutfit">(current_player);
					ImGui::SameLine();
					components::button("Copy Vehicle", [current_player] {
						if (Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(current_player->id()), false); veh)
							persist_car_service::clone_ped_car(self::ped, veh);
					});
				}
				ImGui::EndGroup();
			}
			ImGui::EndGroup();

			ImGui::SameLine(0, 100);

			ImGui::BeginGroup();
			{
				components::sub_title("Teleport / Location");

				components::player_command_button<"playertp">(current_player);
				ImGui::SameLine();
				components::player_command_button<"playervehtp">(current_player);
				ImGui::SameLine();
				components::player_command_button<"bring">(current_player);

				components::button("Set Waypoint", [current_player] {
					Vector3 location = ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(current_player->id()), true);
					HUD::SET_NEW_WAYPOINT(location.x, location.y);
				});
			}
			ImGui::EndGroup();

			ImGui::SameLine(0, 100);

			ImGui::BeginGroup();
			{
				ImGui::BeginGroup();
				{
					components::sub_title("Kick");

					if (g_player_service->get_self()->is_host())
						components::player_command_button<"hostkick">(current_player);
					else
					{
						ImGui::BeginGroup();
						{
							components::player_command_button<"shkick">(current_player);
							components::player_command_button<"endkick">(current_player);
							ver_Space();
							components::player_command_button<"nfkick">(current_player);
							components::player_command_button<"oomkick">(current_player);
							if (!current_player->is_host())
								components::player_command_button<"desync">(current_player);
						}
						ImGui::EndGroup();
						ImGui::SameLine(0, 2.0f * ImGui::GetTextLineHeight());
						ImGui::BeginGroup();
						{
							components::player_command_button<"multikick">(current_player);
						}
						ImGui::EndGroup();
					}
				}
				ImGui::EndGroup();
				ver_Space();
				ImGui::BeginGroup();
				{
					components::sub_title("Toxic");
					ImGui::BeginDisabled(globals::get_interior_from_player(current_player->id()) != 0);
					components::player_command_button<"kill">(current_player, {});
					ImGui::EndDisabled();

					components::player_command_button<"vehkick">(current_player, {});
					components::button("Stop Vehicle", [current_player] {
						Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(current_player->id()), false);
						if (veh && ENTITY::IS_ENTITY_A_VEHICLE(veh) && entity::take_control_of(veh))
							VEHICLE::BRING_VEHICLE_TO_HALT(veh, 1, 5, true);
					});
					components::button("Empty Vehicle", [current_player] {
						Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(current_player->id()), false);
						if (veh && ENTITY::IS_ENTITY_A_VEHICLE(veh))
							vehicle::clear_all_peds(veh);
					});
					components::button("Bring Vehicle (only)", [current_player] {
						Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(current_player->id()), false);
						vehicle::bring(veh, self::pos, false);
					});
					ver_Space();
					components::button("Delete Vehicle", [current_player] {
						Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(current_player->id()), false);
						entity::delete_entity(veh);
					});
				}
				ImGui::EndGroup();
			}
			ImGui::EndGroup();
		}
	}
}