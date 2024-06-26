#pragma once
#include "blocked_players.hpp"

#include "logger/logger.hpp"

using json = nlohmann::json;

namespace big
{
	std::filesystem::path blocked_players_service::get_file_path()
	{
		return g_file_manager.get_project_file("blocked_players.json").get_path();
	}

	void blocked_players_service::add_player(rock_id rockstar_id, blocked_player player)
	{
		blocked_players_list[rockstar_id] = player;
		if (player.block_join)
			++save_count;
	}

	void blocked_players_service::add_player(player_ptr player, bool block_join, bool is_spammer)
	{
		add_player(player->m_rockstar_id, {player->m_name, block_join, is_spammer, is_spammer ? "spam" : player->spam_message});
	}

	void blocked_players_service::toggle_block(rock_id rockstar_id, bool v)
	{
		blocked_players_list[rockstar_id].block_join = v;
		++save_count;
	}

	bool blocked_players_service::is_blocked(rock_id rockstar_id)
	{
		auto blocked_player = blocked_players_list.find(rockstar_id);
		return blocked_player != blocked_players_list.end() && blocked_player->second.block_join;
	}

	bool blocked_players_service::does_exist(rock_id rockstar_id)
	{
		auto blocked_player = blocked_players_list.find(rockstar_id);
		return blocked_player != blocked_players_list.end();
	}

	void blocked_players_service::load_blocked_list()
	{
		try
		{
			if (auto filePath = get_file_path(); std::filesystem::exists(filePath))
			{
				std::ifstream f(filePath);

				if (f.is_open())
				{
					blocked_players_list = json::parse(f);
					f.close();
				}
			}
		}
		catch (std::exception e)
		{
			LOG(WARNING) << e.what();
		}
	}

	void blocked_players_service::save_blocked_list()
	{
		std::map<uint64_t, blocked_player> filtered_map;
		for (const auto& entry : blocked_players_list)
			if (entry.second.block_join)
				filtered_map[entry.first] = entry.second;

		try
		{
			json j = filtered_map;
			if (std::ofstream o(get_file_path()); o.is_open())
			{
				o << std::setw(4) << j << std::endl;
				o.close();
			}
		}
		catch (const nlohmann::detail::type_error& e)
		{
			LOG(WARNING) << e.what();
		}
		catch (std::exception e)
		{
			LOG(WARNING) << e.what();
		}
	}
}