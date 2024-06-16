#include "core/data/language_codes.hpp"
#include "core/data/region_codes.hpp"
#include "core/data/session.hpp"
#include "fiber_pool.hpp"
#include "gta_util.hpp"
#include "util/chat.hpp"
#include "util/scripts.hpp"
#include "util/session.hpp"
#include "views/view.hpp"

namespace big
{
	constexpr auto max_token_val = 18000000000000000000;

	static inline void render_region_switcher()
	{
		components::sub_title("Region Switcher");

		if (g_pointers->m_gta.m_region_code)
		{
			ImGui::SetNextItemWidth(200.f);
			if (ImGui::BeginCombo("##regionswitcher", regions[*g_pointers->m_gta.m_region_code].name))
			{
				for (const auto& region_type : regions)
					components::selectable(region_type.name, *g_pointers->m_gta.m_region_code == region_type.id, [&region_type] {
						*g_pointers->m_gta.m_region_code = region_type.id;
					});

				ImGui::EndCombo();
			}
		}

		ImGui::Spacing();

		ImGui::SetNextItemWidth(200.f);
		if (ImGui::BeginCombo("##natswitcher", nat_types[g_session.nat_type].name))
		{
			for (const auto& nat_type : nat_types)
				if (components::selectable(nat_type.name, g_session.nat_type == nat_type.id))
					g_session.nat_type = nat_type.id;
			ImGui::EndCombo();
		}
	}

	static inline void render_misc()
	{
		components::sub_title("Misc");

		ImGui::Checkbox("Block Joins", &g_session.block_joins);
		ImGui::Checkbox("Block Friend Joins", &g_session.block_friend_joins);

		ImGui::Checkbox("Auto Kick Chat Spammers", &g_session.auto_kick_chat_spammers);

		ImGui::Checkbox("Block Muggers", &g_session.block_muggers);
		ImGui::Checkbox("Unhide Players From List", &g_session.unhide_players_from_player_list);

		components::script_patch_checkbox("Reveal OTR Players", &g_session.decloak_players);
	}

	static inline void render_session_multiplexer()
	{
		components::sub_title("Session Multiplexer");

		ImGui::Checkbox("Multiplex Session", &g_session.multiplex_session);
		if (g_session.multiplex_session)
		{
			ImGui::SameLine();
			ImGui::SetNextItemWidth(100);
			ImGui::SliderInt("###multiplex_cnt", &g_session.multiplex_count, 2, 5);
		}
		ImGui::Checkbox("Spoof Session Player Counts", &g_session.spoof_session_player_count);
		if (g_session.spoof_session_player_count)
		{
			ImGui::SameLine();
			ImGui::SetNextItemWidth(100);
			ImGui::InputInt("###Spoofplayer_count", &g_session.session_player_count);
		}
		ImGui::Checkbox("Spoof region", &g_session.spoof_session_region_type);
		if (g_session.spoof_session_region_type)
		{
			ImGui::SetNextItemWidth(200.f);
			if (ImGui::BeginCombo("##spoofedregionswitcher", regions[g_session.session_region_type].name))
			{
				for (const auto& region_type : regions)
					if (components::selectable(region_type.name, g_session.session_region_type == region_type.id))
						g_session.session_region_type = region_type.id;
				ImGui::EndCombo();
			}
		}
		ImGui::Checkbox("Spoof Language", &g_session.spoof_session_language);
		if (g_session.spoof_session_language)
		{
			ImGui::SetNextItemWidth(200.f);
			if (ImGui::BeginCombo("##spoofedlanguage_select", languages[g_session.session_language].name))
			{
				for (const auto& language : languages)
					if (components::selectable(language.name, g_session.session_language == language.id))
						g_session.session_language = language.id;
				ImGui::EndCombo();
			}
		}
	}

	static inline void render_chat()
	{
		components::sub_title("Chat");

		static bool is_team;
		static char msg[256];

		ImGui::Checkbox("Log Chat", &g_session.log_chat);

		ImGui::Spacing();

		ImGui::BeginDisabled(g_session.sending_chat_msg);

		ImGui::SetNextItemWidth(300);
		components::input_text_with_hint("##message", "Message", msg, sizeof(msg));
		ImGui::Checkbox("Is Team Message", &is_team);
		ImGui::SameLine();
		if (components::button("Send Message"))
			chat::send_chat_message(msg, is_team);

		ImGui::EndDisabled();
	}

	static inline void render_hosting()
	{
		components::sub_title("Session Host");

		auto network = gta_util::get_network();

		ImGui::Text("Custom Host Token: ");

		ImGui::BeginDisabled(!g_session.orig_host_token || (network && network->m_game_session_state != 0));

		ImGui::SetNextItemWidth(150);
		if (ImGui::InputScalar("Custom host Token##customhostoken", ImGuiDataType_U64, &g_session.host_token))
			g_fiber_pool->queue_job([] {
				if (g_session.host_token < 0 || g_session.host_token > max_token_val)
					g_session.host_token = max_token_val;

				session::change_host_token();
			});

		components::button("Restore Host token", [] {
			g_session.host_token = g_session.orig_host_token;
			session::change_host_token();
		});

		ImGui::EndDisabled();

		ImGui::Spacing();
		components::button("Clear all players infractions", [] {
			g_player_service->iterate([](const player_entry& player) {
				player.second->infractions.clear();
			});
		});

		ImGui::Spacing();

		components::sub_title("Script Hosts");

		static std::string freemode_sh_name, fmmc_launcher_sh_name, am_launcher_sh_name;

		components::button("Identify all Script Hosts", [] {
			scripts::get_host_name("freemode"_J, freemode_sh_name);
			scripts::get_host_name("fmmc_launcher"_J, fmmc_launcher_sh_name);
			scripts::get_host_name("am_launcher"_J, am_launcher_sh_name);
		});

		ImGui::Text("freemode : %s", freemode_sh_name.c_str());
		ImGui::Text("fmmc_launcher : %s", fmmc_launcher_sh_name.c_str());
		ImGui::Text("am_launcher : %s", am_launcher_sh_name.c_str());

		ImGui::Spacing();

		if (ImGui::Checkbox("Force freemode Host", &g_session.force_freemode_host))
			g_fiber_pool->queue_job([] {
				scripts::force_migration("freemode"_J, g_session.force_freemode_host);
			});
		if (ImGui::Checkbox("Force fmmc_launcher Host", &g_session.force_fmmc_launcher_host))
			g_fiber_pool->queue_job([] {
				scripts::force_migration("fmmc_launcher"_J, g_session.force_fmmc_launcher_host);
			});
		if (ImGui::Checkbox("Force am_launcher Host", &g_session.force_am_launcher_host))
			g_fiber_pool->queue_job([] {
				scripts::force_migration("am_launcher"_J, g_session.force_am_launcher_host);
			});
	}

	static inline void render_host_list()
	{
		components::sub_title("Next Host");

		int hosts_count = 0;
		for (auto& pair : g_session.next_host_list.list)
		{
			ImGui::Text("%s (%llu)", pair.second.name.c_str(), pair.second.token);
			if (++hosts_count; hosts_count == 6)
				break;
		}
	}

	void view::network()
	{
		ImGui::BeginGroup();
		{
			render_region_switcher();
			ImGui::Spacing();
			render_misc();
			ImGui::Spacing();
			render_session_multiplexer();
		}
		ImGui::EndGroup();
		components::hor_space();
		ImGui::BeginGroup();
		{
			render_chat();
			ImGui::Spacing();
			render_hosting();
		}
		ImGui::EndGroup();
		components::hor_space();
		ImGui::BeginGroup();
		{
			render_host_list();
		}
		ImGui::EndGroup();
	}
}
