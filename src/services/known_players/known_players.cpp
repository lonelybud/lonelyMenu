#pragma once
#include "known_players.hpp"

using json = nlohmann::json;

namespace big
{
	std::filesystem::path known_players_service::get_file_path()
	{
		return big::g_file_manager.get_project_file("known_players.json").get_path();
	}

	void known_players_service::load_list()
	{
		try
		{
			if (auto filePath = get_file_path(); std::filesystem::exists(filePath))
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

	void known_players_service::save_list()
	{
		try
		{
			json j = known_players_list;
			if (std::ofstream o(get_file_path()); o.is_open())
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

	void known_players_service::add(player_ptr player)
	{
		if (auto net_data = player->get_net_data())
		{
			auto rockstar_id                = net_data->m_gamer_handle.m_rockstar_id;
			auto name                       = net_data->m_name;
			known_players_list[rockstar_id] = {name, rockstar_id};
			++save_count;
		}
	}

	void known_players_service::remove(player_ptr player)
	{
		if (auto net_data = player->get_net_data())
		{
			auto rockstar_id = net_data->m_gamer_handle.m_rockstar_id;
			known_players_list.erase(rockstar_id);
			++save_count;
		}
	}

	bool known_players_service::is_known(uint64_t rockstar_id)
	{
		return known_players_list.find(rockstar_id) != known_players_list.end();
	}
}