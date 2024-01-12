#include "core/data/session.hpp"
#include "fiber_pool.hpp"
#include "gta_util.hpp"
#include "services/bad_players/bad_players.hpp"
#include "util/scripts.hpp"
#include "views/view.hpp"

namespace big
{
	static inline void ver_Space()
	{
		ImGui::Dummy(ImVec2(0.0f, ImGui::GetTextLineHeight()));
	}

	static inline void render_misc()
	{
		ImGui::BeginGroup();
		{
			components::sub_title("Misc");

			ImGui::BeginDisabled(!g_player_service->get_self()->is_host());
			ImGui::Checkbox("Lock Session", &g_session.lock_session);
			ImGui::EndDisabled();

			ImGui::Checkbox("Auto Kick Chat Spammers", &g_session.auto_kick_chat_spammers);

			components::script_patch_checkbox("Reveal OTR Players", &g_session.decloak_players);
		}
		ImGui::EndGroup();
	}

	static inline void render_hosting()
	{
		ImGui::BeginGroup();
		{
			components::sub_title("Session Host");

			ImGui::SetNextItemWidth(150);
			ImGui::InputScalar("Custom host Token##customhostoken", ImGuiDataType_U64, &g_session.host_token);
			if (components::button("Restore Host token"))
				g_session.host_token = g_session.orig_host_token;

			if (g_player_service->get_self()->is_host())
			{
				ImGui::Spacing();
				components::button("Kick all blocked players", [] {
					g_player_service->iterate([](const player_entry& player) {
						if (player.second->is_blocked)
							dynamic_cast<player_command*>(command::get(RAGE_JOAAT("hostkick")))->call(player.second);
					});
				});
			}

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
				scripts::get_host_name(RAGE_JOAAT("freemode"), freemode_sh_name);
				scripts::get_host_name(RAGE_JOAAT("fmmc_launcher"), fmmc_launcher_sh_name);
				scripts::get_host_name(RAGE_JOAAT("am_launcher"), am_launcher_sh_name);
			});

			ImGui::Text(std::format("freemode : {}", freemode_sh_name).c_str());
			ImGui::Text(std::format("fmmc_launcher : {}", fmmc_launcher_sh_name).c_str());
			ImGui::Text(std::format("am_launcher : {}", am_launcher_sh_name).c_str());

			ImGui::Spacing();

			ImGui::Checkbox("Force Script Host", &g_session.force_script_host);

			components::button("Migrate scripts host", [] {
				scripts::force_migration("freemode", RAGE_JOAAT("freemode"), g_session.force_script_host);
				scripts::force_migration("fmmc_launcher", RAGE_JOAAT("fmmc_launcher"), g_session.force_script_host);
				scripts::force_migration("am_launcher", RAGE_JOAAT("am_launcher"), g_session.force_script_host);
			});
		}
		ImGui::EndGroup();
	}

	static inline void render_host_list()
	{
		ImGui::BeginGroup();
		{
			components::sub_title("Smallest Host token");
			ImGui::SameLine();
			if (ImGui::SmallButton("reset##resetsmallToken"))
			{
				g_session.smallest_host_token       = g_session.orig_host_token;
				g_session.smallest_host_token_owner = "";
			}

			ImGui::Text(std::format("Token: {}", g_session.smallest_host_token).c_str());
			ImGui::SameLine();
			if (ImGui::SmallButton("copy##copysmallToken"))
				ImGui::SetClipboardText(std::format("{}", g_session.smallest_host_token).c_str());

			ImGui::Text(std::format("Owner: {}", g_session.smallest_host_token_owner).c_str());

			ImGui::Spacing();

			components::sub_title("Next Host");

			for (auto& pair : g_session.next_host_list.list)
				ImGui::Text(pair.second.name.c_str());
		}
		ImGui::EndGroup();
	}

	void view::network()
	{
		ImGui::BeginGroup();
		{
			render_misc();
		}
		ImGui::EndGroup();
		ImGui::SameLine(0, 2.0f * ImGui::GetTextLineHeight());
		ImGui::BeginGroup();
		{
			render_hosting();
		}
		ImGui::EndGroup();
		ImGui::SameLine(0, 2.0f * ImGui::GetTextLineHeight());
		ImGui::BeginGroup();
		{
			render_host_list();
		}
		ImGui::EndGroup();
	}
}
