#include "core/data/recent_spoofed_host_tokens.hpp"

#include "pointers.hpp"
#include "services/blocked_players/blocked_players.hpp"
#include "views/view.hpp"

namespace big
{
	static rock_id selected_rid;

	static void remove_selected_rid()
	{
		g_recent_spoofed_host_tokens.erase(selected_rid);
		selected_rid = 0;
	}

	void view::recent_spoofed_host_tokens()
	{
		if (components::button("Add all"))
		{
			for (auto& pair : g_recent_spoofed_host_tokens)
				g_blocked_players_service.add_player(pair.first, {pair.second, true, false, 0, "Spoofed host token"});
			g_recent_spoofed_host_tokens.clear();
		}
		ImGui::SameLine();
		if (components::button("Clear"))
			g_recent_spoofed_host_tokens.clear();

		ImGui::Spacing();

		if (ImGui::BeginListBox("##plr_list", {300, static_cast<float>(*g_pointers->m_gta.m_resolution_y * 0.3)}))
		{
			for (auto& pair : g_recent_spoofed_host_tokens)
				if (ImGui::Selectable(pair.second.c_str(), selected_rid == pair.first))
					selected_rid = pair.first;

			ImGui::EndListBox();
		}

		ImGui::Spacing();

		if (selected_rid && components::button("Add to block list"))
		{
			g_blocked_players_service.add_player(selected_rid, {g_recent_spoofed_host_tokens[selected_rid], true, false, 0, "Spoofed host token"});
			remove_selected_rid();
		}
		ImGui::SameLine();
		if (selected_rid && components::button("Delete"))
			remove_selected_rid();
	}
}