#include "core/data/session.hpp"
#include "fiber_pool.hpp"
#include "gta_util.hpp"
#include "services/bad_players/bad_players.hpp"
#include "util/scripts.hpp"
#include "views/view.hpp"

namespace big
{
	static inline void render_misc()
	{
		components::sub_title("Misc");

		ImGui::BeginDisabled(!g_player_service->get_self()->is_host());
		ImGui::Checkbox("Lock Session", &g_session.lock_session);
		ImGui::EndDisabled();

		ImGui::Checkbox("Auto Kick Chat Spammers", &g_session.auto_kick_chat_spammers);

		components::script_patch_checkbox("Reveal OTR Players", &g_session.decloak_players);
	}

	static inline void render_hosting()
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

		ImGui::Text("freemode : %s", freemode_sh_name.c_str());
		ImGui::Text("fmmc_launcher : %s", fmmc_launcher_sh_name.c_str());
		ImGui::Text("am_launcher : %s", am_launcher_sh_name.c_str());

		ImGui::Spacing();

		ImGui::Checkbox("Force Script Host", &g_session.force_script_host);

		components::button("Migrate scripts host", [] {
			scripts::force_migration("freemode", RAGE_JOAAT("freemode"), g_session.force_script_host);
			scripts::force_migration("fmmc_launcher", RAGE_JOAAT("fmmc_launcher"), g_session.force_script_host);
			scripts::force_migration("am_launcher", RAGE_JOAAT("am_launcher"), g_session.force_script_host);
		});
	}

	static inline void render_host_list()
	{
		components::sub_title("Next Host");

		components::button("Refresh###hostlist", [] {
			big::player_ptr host = nullptr;

			if (g_player_service->get_self()->is_host())
				host = g_player_service->get_self();
			else
				for (const auto& [_, plyr] : g_player_service->players())
					if (plyr->is_host())
					{
						host = plyr;
						break;
					}

			if (host)
				g_session.next_host_list.delete_plyr(host->id()); // filter out host from the list
		});

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
			render_misc();
		}
		ImGui::EndGroup();
		components::hor_space();
		ImGui::BeginGroup();
		{
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
