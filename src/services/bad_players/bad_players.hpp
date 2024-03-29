#pragma once
#include "services/players/player_service.hpp"

namespace big
{
	struct bad_player
	{
		std::string n; // name
		bool block_join = true;
		bool s;        // spammer
		int l;         // language
		std::string m; // message
	};
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(bad_player, n, s, l, m);

	class bad_players_service
	{
	public:
		int save_count;
		std::map<uint64_t, bad_player> bad_players_list;

		std::filesystem::path get_file_path();
		void add_player(uint64_t rockstar_id, bad_player player);
		void add_player(player_ptr player, bool block_join, bool is_spammer);
		void toggle_block(uint64_t rockstar_id, bool v);
		bool is_blocked(uint64_t rockstar_id);
		bool does_exist(uint64_t rockstar_id);
		void load_blocked_list();
		void save_blocked_list();
	};

	inline bad_players_service g_bad_players_service;
}