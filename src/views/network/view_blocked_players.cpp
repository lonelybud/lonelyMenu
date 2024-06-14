#include "core/data/language_codes.hpp"
#include "pointers.hpp"
#include "services/blocked_players/blocked_players.hpp"
#include "services/notifications/notification_service.hpp"
#include "util/strings.hpp"
#include "views/view.hpp"

static constexpr int msg_size = 320;

namespace big
{
	static char player_name[64];
	static rock_id rockstar_id;
	static int language;
	static bool save_as_spammer, block_join;
	static char message[msg_size];
	static bool exist_already;

	static void set_selected(rock_id rid, blocked_player p)
	{
		strcpy(player_name, p.n.c_str());
		rockstar_id     = rid;
		language        = p.l;
		save_as_spammer = p.s;
		exist_already = block_join = p.block_join;
		strcpy_safe(message, p.m.c_str(), msg_size);
	}

	static inline std::unordered_map<rock_id, blocked_player> filter_blocked_players(const std::unordered_map<rock_id, blocked_player>& inputMap, const std::string& search)
	{
		std::unordered_map<rock_id, blocked_player> res;

		for (auto pair : inputMap)
		{
			auto t = pair.second.n;
			std::transform(t.begin(), t.end(), t.begin(), ::tolower);

			if (t.find(search) != std::string::npos)
				res[pair.first] = pair.second;
		}

		return res;
	}

	void view::blocked_players()
	{
		static std::unordered_map<rock_id, blocked_player> searched_blocked_players;
		static std::string search_blocked_player_name;

		ImGui::PushItemWidth(300);
		components::input_text("Player Name", player_name, sizeof(player_name));
		ImGui::InputScalar("Rockstar Id", ImGuiDataType_U64, &rockstar_id);
		ImGui::PopItemWidth();
		ImGui::SetNextItemWidth(200.f);
		if (ImGui::BeginCombo("##languages", language == -1 ? "Unknown" : languages[language].name))
		{
			if (components::selectable("Unknown", language == -1))
				language = -1;

			for (const auto& lang : languages)
				if (components::selectable(lang.name, language == lang.id))
					language = lang.id;

			ImGui::EndCombo();
		}

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
			if (components::button("Save"))
			{
				block_join = true;
				g_blocked_players_service.add_player(rockstar_id, {player_name, block_join, save_as_spammer, language, message});
			}
			ImGui::SameLine();
			if (components::button("Un-block"))
			{
				g_blocked_players_service.toggle_block(rockstar_id, false);
				exist_already = false;
			}
		}
		else if (components::button("Add to block list"))
		{
			std::string name = player_name;
			if (trimString(name).length() && rockstar_id)
			{
				block_join = true;
				g_blocked_players_service.add_player(rockstar_id, {player_name, block_join, save_as_spammer, language, message});
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
				searched_blocked_players = filter_blocked_players(g_blocked_players_service.blocked_players_list, search_blocked_player_name);
			}
			else
				searched_blocked_players.clear();
		}

		ImGui::Spacing();

		ImGui::BeginGroup();
		{
			ImGui::Text("Joins Not blocked -");
			if (ImGui::BeginListBox("##blocked_players", {300, static_cast<float>(*g_pointers->m_gta.m_resolution_y * 0.3)}))
			{
				for (auto& pair : g_blocked_players_service.blocked_players_list)
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
			if (ImGui::BeginListBox("##blocked_players_blocked", {300, static_cast<float>(*g_pointers->m_gta.m_resolution_y * 0.3)}))
			{
				for (auto& pair :
				    (search_blocked_player_name.length() > 0 ? searched_blocked_players : g_blocked_players_service.blocked_players_list))
					if (pair.second.block_join && ImGui::Selectable(pair.second.n.c_str(), rockstar_id && rockstar_id == pair.first))
						set_selected(pair.first, pair.second);

				ImGui::EndListBox();
			}
		}
		ImGui::EndGroup();

		components::button("Reset Non Blocked List", [] {
			set_selected(0, {});
			for (auto it = g_blocked_players_service.blocked_players_list.begin(); it != g_blocked_players_service.blocked_players_list.end();)
				if (!it->second.block_join)
					it = g_blocked_players_service.blocked_players_list.erase(it);
				else
					++it;
		});
		ImGui::SameLine();
		components::button("Reset All", [] {
			set_selected(0, {});
			g_blocked_players_service.load_blocked_list();
		});
	}
}