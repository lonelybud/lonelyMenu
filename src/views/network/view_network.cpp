#include "core/data/language_codes.hpp"
#include "core/data/player.hpp"
#include "core/data/region_codes.hpp"
#include "core/data/session.hpp"
#include "fiber_pool.hpp"
#include "gta_util.hpp"
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

	static inline void render_session_switcher()
	{
		components::sub_title("Session Switcher");

		components::button("Start New Public", [] {
			session::join_type(eSessionType::NEW_PUBLIC);
		});
		ImGui::SameLine();
		components::button("Leave Online", [] {
			session::join_type(eSessionType::LEAVE_ONLINE);
		});

		ImGui::Spacing();

		components::button("Join Last Session", [] {
			session::join_session(gta_util::get_network()->m_last_joined_session.m_session_info);
		});

		ImGui::Spacing();

		static char base64[500]{};

		ImGui::SetNextItemWidth(200);
		components::input_text_with_hint("##sessioninfoinput", "Session Info", base64, sizeof(base64));
		ImGui::SameLine();
		components::button("Join Session", [] {
			rage::rlSessionInfo info;
			if (g_pointers->m_gta.m_decode_session_info(&info, base64, nullptr))
				session::join_session(info);
			else
				g_notification_service.push_error("Join session", "Invalid info");
		});
	}

	static inline void render_remote_player()
	{
		components::sub_title("Remote Player");

		static rock_id rid = 0;

		ImGui::SetNextItemWidth(200);
		ImGui::InputScalar("##inputrid", ImGuiDataType_U64, &rid);
		components::button("Join RID", [] {
			session::join_by_rockstar_id(rid);
		});
		ImGui::SameLine();
		components::button("Invite", [] {
			session::invite_by_rockstar_id(rid);
		});
	}

	static inline void render_misc()
	{
		components::sub_title("Misc");

		ImGui::Checkbox("Block Joins", &g_session.block_joins);
		ImGui::Checkbox("Block Friend Joins", &g_session.block_friend_joins);
		ImGui::Checkbox("Block Spoofed tokens", &g_session.block_spoofed_tokens);
		ImGui::Checkbox("Auto Kick Chat Spammers", &g_session.auto_kick_chat_spammers);
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
			if (ImGui::BeginCombo("##spoofedlanguage_select", languages.at(g_session.session_language).data()))
			{
				for (const auto& language : languages)
					if (components::selectable(language.second, g_session.session_language == language.first))
						g_session.session_language = language.first;
				ImGui::EndCombo();
			}
		}
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

		ImGui::Text("freemode : %s", scripts::get_host_name("freemode"_J));
		ImGui::Text("fmmc_launcher : %s", scripts::get_host_name("fmmc_launcher"_J));
		ImGui::Text("am_launcher : %s", scripts::get_host_name("am_launcher"_J));

		ImGui::Spacing();

		ImGui::Checkbox("Force freemode Host", &g_session.force_freemode_host);
		ImGui::Checkbox("Force fmmc_launcher Host", &g_session.force_fmmc_launcher_host);
		ImGui::Checkbox("Force am_launcher Host", &g_session.force_am_launcher_host);
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
			render_session_switcher();
			ImGui::Spacing();
			render_remote_player();
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
