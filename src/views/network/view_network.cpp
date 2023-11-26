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
		ImGui::BeginGroup();
		{
			components::sub_title("Misc");

			ImGui::BeginDisabled(!g_player_service->get_self()->is_host());
			ImGui::Checkbox("Lock Session", &g_session.lock_session);
			ImGui::EndDisabled();
		}
		ImGui::EndGroup();
	}

	static inline void render_hosting()
	{
		ImGui::BeginGroup();
		{
			components::sub_title("Hosting");

			ImGui::SetNextItemWidth(150);
			ImGui::InputScalar("Custom host Token##customhostoken", ImGuiDataType_U64, &g_session.custom_host_token);

			ImGui::Spacing();

			static std::string script_host_player_name = "";
			components::button("Identify Script Host", [] {
				if (auto launcher = gta_util::find_script_thread(RAGE_JOAAT("freemode")); launcher && launcher->m_net_component)
					if (auto host = ((CGameScriptHandlerNetComponent*)launcher->m_net_component)->get_host(); host)
						script_host_player_name = host->get_name();
			});
			ImGui::SameLine();
			ImGui::Text(script_host_player_name.c_str());

			components::button("Become Script Host", [] {
				scripts::force_migration(RAGE_JOAAT("freemode"));
			});

			if (g_player_service->get_self()->is_host())
			{
				ImGui::Spacing();
				components::button("Kick all blocked players", [] {
					g_player_service->iterate([](const player_entry& player) {
						if (player.second->is_blocked)
							dynamic_cast<player_command*>(command::get(RAGE_JOAAT("hostkick")))->call(player.second, {});
					});
				});
			}
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
	}
}
