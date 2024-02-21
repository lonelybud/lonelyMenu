#pragma once
#include "services/players/player_service.hpp"

using json = nlohmann::json;

namespace big::known_player_nm
{
	struct known_player
	{
		std::string name;
		uint64_t rockstar_id;
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(known_player, name, rockstar_id);

	inline std::unordered_map<uint64_t, known_player> known_players_list;

	inline std::filesystem::path getSavedFilePath()
	{
		return big::g_file_manager.get_project_file("known_players.json").get_path();
	}

	inline void load_list()
	{
		try
		{
			if (auto filePath = getSavedFilePath(); std::filesystem::exists(filePath))
			{
				std::ifstream f(filePath);

				if (f.is_open())
				{
					known_players_list = json::parse(f);
					f.close();
				}
			}
		}
		catch (std::exception e)
		{
			LOG(WARNING) << e.what();
		}
	}

	inline void save_list()
	{
		try
		{
			json j = known_players_list;
			if (std::ofstream o(getSavedFilePath()); o.is_open())
			{
				o << std::setw(4) << j << std::endl;
				o.close();
			}
		}
		catch (std::exception e)
		{
			LOG(WARNING) << e.what();
		}
	}

	inline void toggle(player_ptr player, bool add = false)
	{
		if (auto net_data = player->get_net_data())
		{
			auto rockstar_id = net_data->m_gamer_handle.m_rockstar_id;
			auto name        = net_data->m_name;
			auto it          = known_players_list.find(rockstar_id);

			if (!add && it != known_players_list.end())
				known_players_list.erase(it);
			else
				known_players_list[rockstar_id] = {name, rockstar_id};

			save_list();
		}
	}
	inline bool is_known(uint64_t rockstar_id)
	{
		return known_players_list.find(rockstar_id) != known_players_list.end();
	}
}