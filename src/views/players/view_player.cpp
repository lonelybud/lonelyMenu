#include "core/data/infractions.hpp"
#include "core/data/language_codes.hpp"
#include "core/scr_globals.hpp"
#include "core/settings/protections.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "services/bad_players/bad_players.hpp"
#include "services/gui/gui_service.hpp"
#include "services/vehicle/persist_car_service.hpp"
#include "util/globals.hpp"
#include "util/player.hpp"
#include "util/scripts.hpp"
#include "views/view.hpp"

#include <network/netConnection.hpp>
#include <script/globals/GPBD_FM.hpp>
#include <script/globals/GPBD_FM_3.hpp>
#include <script/globals/GlobalPlayerBD.hpp>

static constexpr char ip_viewer_link[] = "https://iplogger.org/ip-tracker/?ip=";

namespace big
{
	const char* get_nat_type_str(int type)
	{
		switch (type)
		{
		case 1: return "open";
		case 2: return "moderate";
		case 3: return "strict";
		}
		return "unkown";
	}

	static big::player_ptr last_selected_player;
	static uint64_t rockstar_id;

	static inline void ver_Space()
	{
		ImGui::Dummy(ImVec2(0.0f, ImGui::GetTextLineHeight()));
	}

	static void extra_info_button(player_ptr current_player)
	{
		components::options_modal(
		    "Extra Info",
		    [current_player] {
			    if (auto id = current_player->id(); id != -1)
			    {
				    ImGui::BeginGroup();
				    {
					    auto& stats       = scr_globals::gpbd_fm_1.as<GPBD_FM*>()->Entries[id].PlayerStats;
					    auto& boss_goon   = scr_globals::gpbd_fm_3.as<GPBD_FM_3*>()->Entries[id].BossGoon;
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

					    ImGui::Text(std::format("NAT Type: {}",
					        get_nat_type_str(g_player_service->get_selected()->get_net_data()->m_nat_type))
					                    .c_str());

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
						    {
							    ImGui::SameLine();
							    if (ImGui::SmallButton("copy##copyip"))
								    ImGui::SetClipboardText(std::format("{}{}.{}.{}.{}", ip_viewer_link, ip.m_field1, ip.m_field2, ip.m_field3, ip.m_field4)
								                                .c_str());
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

				    ImGui::Separator();

				    if (auto net_data = current_player->get_net_data())
				    {
					    ImGui::Text(std::format("Host token: {}", net_data->m_host_token).c_str());
					    ImGui::SameLine();
					    if (ImGui::SmallButton("copy##copyHtoken"))
						    ImGui::SetClipboardText(std::format("{}", net_data->m_host_token).c_str());
				    }
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
			if (components::button("Copy Name##copyname"))
				ImGui::SetClipboardText(current_player->get_name());

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
			components::sub_title("Teleport / Location");

			components::button("Set Waypoint", [current_player] {
				Vector3 location = ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(current_player->id()), true);
				HUD::SET_NEW_WAYPOINT(location.x, location.y);
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
			ver_Space();

			components::button("Copy Vehicle", [current_player] {
				if (Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(current_player->id()), false); veh)
					persist_car_service::clone_ped_car(veh);
			});
			ImGui::SameLine();
			components::button("Save Vehicle", [current_player] {
				if (Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(current_player->id()), false); veh)
				{
					auto currentTime = std::chrono::system_clock::now().time_since_epoch();
					auto millis      = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime).count();
					persist_car_service::save_vehicle(veh, std::to_string(millis).append(".json"), "");
				}
			});
		}
		ImGui::EndGroup();
	}

	static inline void render_kick(player_ptr current_player)
	{
		ImGui::BeginGroup();
		if (!current_player->is_host() || *g_pointers->m_gta.m_is_session_started) // to prevent you from kicking host when session has not started.
		{
			components::sub_title("Kick");

			if (g_player_service->get_self()->is_host())
				components::player_command_button<"hostkick">(current_player);

			ImGui::Spacing();

			components::player_command_button<"shkick">(current_player);
			components::player_command_button<"endkick">(current_player);
			components::player_command_button<"nfkick">(current_player);
			components::player_command_button<"oomkick">(current_player);

			ImGui::Spacing();

			if (!current_player->is_host())
				components::player_command_button<"desync">(current_player);
		}
		ImGui::EndGroup();
	}

	static inline void render_infractions(player_ptr current_player)
	{
		if (!current_player->infractions.empty())
		{
			ImGui::BeginGroup();
			components::sub_title("Infractions");

			if (current_player->crash_count)
				ImGui::BulletText(std::format("*** Crash Count - {}", current_player->crash_count).c_str());

			for (auto infraction : current_player->infractions)
				ImGui::BulletText(infraction_desc[(Infraction)infraction]);
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

				ver_Space();

				render_chat_spam(current_player);

				ver_Space();

				render_infractions(current_player);
			}
			ImGui::EndGroup();
			ImGui::SameLine(0, 50);
			ImGui::BeginGroup();
			{
				render_tel_loc(current_player);

				ver_Space();

				render_misc(current_player);
			}
			ImGui::EndGroup();
			ImGui::SameLine(0, 50);
			ImGui::BeginGroup();
			{
				render_kick(current_player);
			}
			ImGui::EndGroup();
		}
	}
}