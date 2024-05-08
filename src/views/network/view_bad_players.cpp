#include "pointers.hpp"
#include "services/bad_players/bad_players.hpp"
#include "services/notifications/notification_service.hpp"
#include "util/strings.hpp"
#include "views/view.hpp"

static constexpr int msg_size = 320;

namespace big
{
	static char player_name[64];
	static int64_t rockstar_id;
	static int language;
	static bool save_as_spammer, block_join;
	static char message[msg_size];
	static bool exist_already;

	static void set_selected(int64_t rid, bad_player p)
	{
		strcpy(player_name, p.n.c_str());
		rockstar_id     = rid;
		language        = p.l;
		save_as_spammer = p.s;
		exist_already = block_join = p.block_join;
		strcpy_safe(message, p.m.c_str(), msg_size);
	}

	static inline std::unordered_map<int64_t, bad_player> filter_bad_players(const std::unordered_map<int64_t, bad_player>& inputMap, const std::string& search)
	{
		std::unordered_map<int64_t, bad_player> res;

		for (auto pair : inputMap)
		{
			auto t = pair.second.n;
			std::transform(t.begin(), t.end(), t.begin(), ::tolower);

			if (t.find(search) != std::string::npos)
				res[pair.first] = pair.second;
		}

		return res;
	}

	void view::bad_players()
	{
		static std::unordered_map<int64_t, bad_player> searched_blocked_players;
		static std::string search_blocked_player_name;

		ImGui::PushItemWidth(300);
		components::input_text("Player Name", player_name, sizeof(player_name));
		ImGui::InputScalar("Rockstar Id", ImGuiDataType_U64, &rockstar_id);
		ImGui::InputScalar("Language", ImGuiDataType_U32, &language);
		ImGui::PopItemWidth();

		ImGui::Checkbox("Save as spammer", &save_as_spammer);
		ImGui::Checkbox("Block Join", &block_join);

		ImGui::SetNextItemWidth(500);
		components::input_text("Message##1", message, sizeof(message));

		ImGui::Spacing();

		components::button("Reset Form", [] {
			set_selected(0, {});
		});

		if (exist_already)
		{
			if (components::button("Un-block"))
			{
				g_bad_players_service.toggle_block(rockstar_id, false);
				exist_already = false;
			}
		}
		else if (components::button("Add to block list"))
		{
			std::string name = player_name;
			if (trimString(name).length() && rockstar_id)
			{
				block_join = true;
				g_bad_players_service.add_player(rockstar_id, {player_name, block_join, save_as_spammer, language, message});
				save_as_spammer = false;
				strcpy(player_name, "");
				rockstar_id = 0;
				searched_blocked_players.clear();
			}
			else
				g_notification_service.push_error("New Player Entry", "Player Name or Rockstar Id is missing.");
		}

		ImGui::Spacing();

		ImGui::SetNextItemWidth(300);
		if (components::input_text_with_hint("###searched_blocked_players", "search blocked player", search_blocked_player_name))
		{
			if (search_blocked_player_name.length() > 0)
			{
				std::transform(search_blocked_player_name.begin(),
				    search_blocked_player_name.end(),
				    search_blocked_player_name.begin(),
				    ::tolower);
				searched_blocked_players = filter_bad_players(g_bad_players_service.bad_players_list, search_blocked_player_name);
			}
			else
				searched_blocked_players.clear();
		}

		ImGui::Spacing();

		ImGui::BeginGroup();
		{
			ImGui::Text("Joins Not blocked -");
			if (ImGui::BeginListBox("##bad_players", {300, static_cast<float>(*g_pointers->m_gta.m_resolution_y * 0.3)}))
			{
				for (auto& pair : g_bad_players_service.bad_players_list)
					if (!pair.second.block_join && ImGui::Selectable(pair.second.n.c_str(), rockstar_id && rockstar_id == pair.first))
						set_selected(pair.first, pair.second);

				ImGui::EndListBox();
			}
		}
		ImGui::EndGroup();
		ImGui::SameLine();
		ImGui::BeginGroup();
		{
			ImGui::Text("Joins blocked -");
			if (ImGui::BeginListBox("##bad_players_blocked", {300, static_cast<float>(*g_pointers->m_gta.m_resolution_y * 0.3)}))
			{
				for (auto& pair :
				    (search_blocked_player_name.length() > 0 ? searched_blocked_players : g_bad_players_service.bad_players_list))
					if (pair.second.block_join && ImGui::Selectable(pair.second.n.c_str(), rockstar_id && rockstar_id == pair.first))
						set_selected(pair.first, pair.second);

				ImGui::EndListBox();
			}
		}
		ImGui::EndGroup();

		components::button("Reset Non Blocked List", [] {
			set_selected(0, {});
			for (auto it = g_bad_players_service.bad_players_list.begin(); it != g_bad_players_service.bad_players_list.end();)
				if (!it->second.block_join)
					it = g_bad_players_service.bad_players_list.erase(it);
				else
					++it;
		});
		ImGui::SameLine();
		components::button("Reset All", [] {
			set_selected(0, {});
			g_bad_players_service.load_blocked_list();
		});
	}
}