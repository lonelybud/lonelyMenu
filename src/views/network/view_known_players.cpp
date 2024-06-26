#include "pointers.hpp"
#include "services/blocked_players/blocked_players.hpp"
#include "services/known_players/known_players.hpp"
#include "services/notifications/notification_service.hpp"
#include "util/strings.hpp"
#include "views/view.hpp"

static constexpr int msg_size = 320;

namespace big
{
	static char player_name[64];
	static rock_id rockstar_id;
	static char message[msg_size];
	static bool exist_already;

	static void set_selected(rock_id rid, known_player p, bool exist)
	{
		strcpy(player_name, p.n.c_str());
		rockstar_id   = rid;
		exist_already = exist;
		strcpy_safe(message, p.m.c_str(), msg_size);
	}

	static inline std::unordered_map<rock_id, known_player> filter_players(const std::unordered_map<rock_id, known_player>& inputMap, const std::string& search)
	{
		std::unordered_map<rock_id, known_player> res;

		for (auto pair : inputMap)
		{
			auto t = pair.second.n;
			std::transform(t.begin(), t.end(), t.begin(), ::tolower);

			if (t.find(search) != std::string::npos)
				res[pair.first] = pair.second;
		}

		return res;
	}

	void view::known_players()
	{
		static std::unordered_map<rock_id, known_player> searched_players;
		static std::string search_player_name;

		ImGui::PushItemWidth(300);
		components::input_text("Player Name", player_name, sizeof(player_name));
		ImGui::InputScalar("Rockstar Id", ImGuiDataType_U64, &rockstar_id);
		ImGui::PopItemWidth();
		ImGui::SetNextItemWidth(500);
		components::input_text("Message##1", message, sizeof(message));

		ImGui::Spacing();

		if (exist_already)
		{
			if (components::button("Save"))
			{
				g_known_players_service.add(player_name, rockstar_id, message);
			}
			ImGui::SameLine();
			if (components::button("Remove"))
			{
				g_known_players_service.remove(rockstar_id);
				exist_already = false;
			}
			ImGui::SameLine();
			if (components::button("Move to block list"))
			{
				g_blocked_players_service.add_player(rockstar_id, {player_name, true, false, message});
				g_known_players_service.remove(rockstar_id);
				set_selected(0, {}, false);
			}
		}
		else if (components::button("Add"))
		{
			std::string name = player_name;
			if (trimString(name).length() && rockstar_id)
			{
				g_known_players_service.add(player_name, rockstar_id, message);
				strcpy(player_name, "");
				rockstar_id = 0;
				searched_players.clear();
			}
			else
				g_notification_service.push_error("New Player Entry", "Player Name or Rockstar Id is missing.");
		}
		ImGui::SameLine();
		components::button("Reset", [] {
			set_selected(0, {}, false);
		});

		ImGui::Spacing();

		ImGui::SetNextItemWidth(300);
		if (components::input_text_with_hint("###searched_players", "search player", search_player_name))
		{
			if (search_player_name.length() > 0)
			{
				std::transform(search_player_name.begin(), search_player_name.end(), search_player_name.begin(), ::tolower);
				searched_players = filter_players(g_known_players_service.known_players_list, search_player_name);
			}
			else
				searched_players.clear();
		}

		ImGui::Spacing();

		ImGui::BeginGroup();
		if (ImGui::BeginListBox("##known_players", {300, static_cast<float>(*g_pointers->m_gta.m_resolution_y * 0.3)}))
		{
			for (auto& pair : (search_player_name.length() > 0 ? searched_players : g_known_players_service.known_players_list))
				if (ImGui::Selectable(pair.second.n.c_str(), rockstar_id && rockstar_id == pair.first))
					set_selected(pair.first, pair.second, true);

			ImGui::EndListBox();
		}
		ImGui::EndGroup();

		components::button("Refresh", [] {
			set_selected(0, {}, false);
			g_known_players_service.load_list();
		});
	}
}