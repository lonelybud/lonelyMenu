#pragma once
#include "services/players/player_service.hpp"

namespace big
{
	struct known_player
	{
		std::string name;
		int64_t rockstar_id;
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(known_player, name, rockstar_id);

	class known_players_service
	{
	public:
		int save_count;
		std::unordered_map<int64_t, known_player> known_players_list;

		std::filesystem::path get_file_path();
		void load_list();
		void save_list();
		inline void add(std::string name, int64_t rockstar_id);
		void add(player_ptr player);
		inline void remove(int64_t rockstar_id);
		void remove(player_ptr player);
		bool is_known(int64_t rockstar_id);
	};

	inline known_players_service g_known_players_service;
}
