#pragma once
#include "fiber_pool.hpp"
#include "services/notifications/notification_service.hpp"
#include "services/players/player_service.hpp"

#include <rage/rlSessionInfo.hpp>

namespace big
{
	class next_host_list
	{
		struct next_host_player
		{
			std::string name;
			uint64_t token;
			bool you;
		};

	public:
		std::vector<std::pair<uint8_t, next_host_player>> list;

		void insert_plyr(uint8_t id, uint64_t token, const char* name, bool you)
		{
			std::pair<uint8_t, next_host_player> element = {id, {name, token, you}};

			// Find the position to insert the element using lower_bound
			auto it = std::lower_bound(list.begin(), list.end(), element, [](auto& a, auto& b) {
				return a.second.token < b.second.token;
			});

			list.insert(it, element);
		}

		void delete_plyr(int8_t id)
		{
			if (list.size())
			{
				auto it = std::remove_if(list.begin(), list.end(), [id](auto& p) {
					return p.first == id;
				});

				if (it != list.end())
					list.erase(it, list.end());
			}
		}
	};

	struct g_session_t
	{
		next_host_list next_host_list;

		uint64_t orig_host_token = 0;
		uint64_t host_token      = 0;
		uint64_t orig_peer_id    = 0;

		bool block_joins          = false;
		bool block_friend_joins   = false;
		bool block_spoofed_tokens = true;

		bool force_freemode_host      = false;
		bool force_fmmc_launcher_host = false;
		bool force_am_launcher_host   = false;

		bool log_chat_messages_to_textbox    = true;
		bool decloak_players                 = false;
		bool auto_kick_chat_spammers         = false;
		bool unhide_players_from_player_list = false;
		int nat_type                         = 4;

		int num_vote_kicks = 0;

		bool multiplex_session          = false;
		int multiplex_count             = 2;
		bool spoof_session_region_type  = false;
		int session_region_type         = 0;
		bool spoof_session_language     = false;
		int session_language            = 0;
		bool spoof_session_player_count = false;
		int session_player_count        = 25;

		bool sending_chat_msg = false;

		bool join_queued = false;
		rage::rlSessionInfo info;
	};

	inline g_session_t g_session{};
}