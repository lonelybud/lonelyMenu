#pragma once
#include "bad_players.hpp"

#include "core/scr_globals.hpp"

#include <script/globals/GPBD_FM_3.hpp>

using json = nlohmann::json;

namespace big
{
	std::filesystem::path bad_players_service::get_file_path()
	{
		return g_file_manager.get_project_file("blocked_players.json").get_path();
	}

	void bad_players_service::add_player(int64_t rockstar_id, bad_player player)
	{
		bad_players_list[rockstar_id] = player;
		if (player.block_join)
			++save_count;
	}

	void bad_players_service::add_player(player_ptr player, bool block_join, bool is_spammer)
	{
		auto& bs = scr_globals::gpbd_fm_3.as<GPBD_FM_3*>()->Entries[player->id()].BossGoon;
		add_player(player->m_rockstar_id,
		    {player->m_name, block_join, is_spammer, bs.Language, is_spammer ? "spam" : player->spam_message});
	}

	void bad_players_service::toggle_block(int64_t rockstar_id, bool v)
	{
		bad_players_list[rockstar_id].block_join = v;
		++save_count;
	}

	bool bad_players_service::is_blocked(int64_t rockstar_id)
	{
		auto bad_player = bad_players_list.find(rockstar_id);
		return bad_player != bad_players_list.end() && bad_player->second.block_join;
	}

	bool bad_players_service::does_exist(int64_t rockstar_id)
	{
		auto bad_player = bad_players_list.find(rockstar_id);
		return bad_player != bad_players_list.end();
	}

	void bad_players_service::load_blocked_list()
	{
		try
		{
			if (auto filePath = get_file_path(); std::filesystem::exists(filePath))
			{
				std::ifstream f(filePath);

				if (f.is_open())
				{
					bad_players_list = json::parse(f);
					f.close();
				}
			}
		}
		catch (std::exception e)
		{
			LOG(WARNING) << e.what();
		}
	}

	void bad_players_service::save_blocked_list()
	{
		std::map<uint64_t, bad_player> filtered_map;
		for (const auto& entry : bad_players_list)
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