#pragma once
#include "core/scr_globals.hpp"
#include "services/players/player_service.hpp"
#include "thread_pool.hpp"

#include <script/globals/GPBD_FM_3.hpp>

using json = nlohmann::json;

namespace big::bad_players_nm
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

	extern std::map<uint64_t, bad_player> bad_players_list;

	inline std::filesystem::path getSavedFilePath()
	{
		return big::g_file_manager.get_project_file("blocked_players.json").get_path();
	}

	void load_blocked_list();
	void save_blocked_list();

	inline void add_player(uint64_t rockstar_id, bad_player player)
	{
		bad_players_list[rockstar_id] = player;
		if (player.block_join)
			big::g_thread_pool->push([] {
				save_blocked_list();
			});
	}

	inline void add_player(player_ptr player, bool block_join, bool is_spammer)
	{
		if (auto net_data = player->get_net_data())
		{
			auto rockstar_id = net_data->m_gamer_handle.m_rockstar_id;
			auto name        = net_data->m_name;
			auto& bs         = scr_globals::gpbd_fm_3.as<GPBD_FM_3*>()->Entries[player->id()].BossGoon;

			add_player(rockstar_id, {name, block_join, is_spammer, bs.Language, is_spammer ? "spam" : player->spam_message});
		}
	}
	inline void toggle_block(uint64_t rockstar_id, bool v)
	{
		bad_players_list[rockstar_id].block_join = v;
		big::g_thread_pool->push([] {
			save_blocked_list();
		});
	}
	inline bool is_blocked(uint64_t rockstar_id)
	{
		auto bad_player = bad_players_list.find(rockstar_id);
		return bad_player != bad_players_list.end() && bad_player->second.block_join;
	}
	inline bool does_exist(uint64_t rockstar_id)
	{
		auto bad_player = bad_players_list.find(rockstar_id);
		return bad_player != bad_players_list.end();
	}
}