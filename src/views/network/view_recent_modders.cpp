#include "pointers.hpp"
#include "services/api/api_service.hpp"
#include "services/notifications/notification_service.hpp"
#include "services/recent_modders/recent_modders.hpp"
#include "thread_pool.hpp"
#include "util/strings.hpp"
#include "views/view.hpp"

inline std::map<uint64_t, recent_modders_nm::recent_modder> filter_blocked_list(const std::map<uint64_t, recent_modders_nm::recent_modder>& inputMap, const std::string& searchString)
{
	std::map<uint64_t, recent_modders_nm::recent_modder> filteredMap;
	std::string lowercaseSearchString = toLowercase(searchString);
	for (auto pair : inputMap)
		if (std::string lowercaseStr = toLowercase(pair.second.name); lowercaseStr.find(lowercaseSearchString) != std::string::npos)
			filteredMap[pair.first] = pair.second;
	return filteredMap;
}

namespace big
{
	void view::recent_modders()
	{
		static char player_name[64];
		static uint64_t selected_id;
		static std::string search_blocked_player_name;
		static bool save_as_spammer;
		std::map<uint64_t, recent_modders_nm::recent_modder> searched_blocked_players;

		ImGui::SetNextItemWidth(300);
		components::input_text("Player Name", player_name, sizeof(player_name));
		ImGui::Checkbox("Save as spammer", &save_as_spammer);
		ImGui::Spacing();
		components::button("Add to block list", [] {
			g_thread_pool->push([] {
				uint64_t rockstar_id;

				if (!g_api_service->get_rid_from_username(player_name, rockstar_id))
					g_notification_service->push_error("New Player Entry", "User could not be found.");
				else
					recent_modders_nm::add_player({player_name, rockstar_id, true, save_as_spammer});

				save_as_spammer = false;
			});
		});
		ImGui::Spacing();

		ImGui::SetNextItemWidth(300);
		if (components::input_text_with_hint("###searched_blocked_players", "search blocked player", search_blocked_player_name))
			searched_blocked_players.clear();

		ImGui::Spacing();
		ImGui::BeginGroup();
		{
			ImGui::Text("Joins Not blocked -");
			if (ImGui::BeginListBox("##recent_modders", {300, static_cast<float>(*g_pointers->m_gta.m_resolution_y * 0.3)}))
			{
				for (auto& pair : recent_modders_nm::recent_modders_list)
					if (!pair.second.block_join && ImGui::Selectable(pair.second.name.c_str(), selected_id && selected_id == pair.first))
						selected_id = pair.first;

				ImGui::EndListBox();
			}
		}
		ImGui::EndGroup();
		ImGui::SameLine();
		ImGui::BeginGroup();
		{
			ImGui::Text("Joins blocked -");
			if (ImGui::BeginListBox("##recent_modders_blocked", {300, static_cast<float>(*g_pointers->m_gta.m_resolution_y * 0.3)}))
			{
				std::map<uint64_t, recent_modders_nm::recent_modder> temp_objs;

				if (searched_blocked_players.size())
					temp_objs = searched_blocked_players;
				else if (search_blocked_player_name.length() > 0)
					temp_objs = searched_blocked_players = filter_blocked_list(recent_modders_nm::recent_modders_list, search_blocked_player_name);

				for (auto& pair : (search_blocked_player_name.length() > 0 ? temp_objs : recent_modders_nm::recent_modders_list))
					if (pair.second.block_join && ImGui::Selectable(pair.second.name.c_str(), selected_id && selected_id == pair.first))
						selected_id = pair.first;

				ImGui::EndListBox();
			}
		}
		ImGui::EndGroup();

		components::button("Reset Non Blocked List", [] {
			selected_id = 0;
			for (auto it = recent_modders_nm::recent_modders_list.begin(); it != recent_modders_nm::recent_modders_list.end();)
			{
				if (!it->second.block_join)
					it = recent_modders_nm::recent_modders_list.erase(it);
				else
					++it;
			}
		});
		components::button("Reset All", [] {
			selected_id = 0;
			recent_modders_nm::load_blocked_list();
		});

		ImGui::Spacing();

		if (selected_id)
		{
			ImGui::Spacing();
			ImGui::Text("Selected Player - ");
			ImGui::Spacing();
			ImGui::Text(recent_modders_nm::recent_modders_list[selected_id].name.c_str());
			ImGui::Text(std::to_string(selected_id).c_str());

			auto block_join = recent_modders_nm::recent_modders_list[selected_id].block_join;
			auto is_spammer = recent_modders_nm::recent_modders_list[selected_id].is_spammer;

			if (ImGui::Checkbox("Block Join", &block_join))
			{
				recent_modders_nm::toggle_block(selected_id);
				searched_blocked_players.clear();
			}
			if (ImGui::Checkbox("Is Spammer", &is_spammer))
				recent_modders_nm::set_spammer(selected_id, is_spammer);
		}
	}
}