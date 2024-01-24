#pragma once
#include "fiber_pool.hpp"
#include "services/notifications/notification_service.hpp"
#include "services/players/player_service.hpp"

namespace big
{
	class next_host_list
	{
		struct next_host_player
		{
			std::string name;
			uint64_t token;
		};

	public:
		std::vector<std::pair<uint8_t, next_host_player>> list;

		void insert_plyr(uint8_t id, uint64_t token, const char* name)
		{
			std::pair<uint8_t, next_host_player> element = {id, {name, token}};

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
		bool lock_session        = false;
		bool force_script_host   = false;

		bool log_chat_messages_to_textbox = true;
		bool decloak_players              = false;
		bool auto_kick_chat_spammers      = true;

		bool notified_as_host = false;
	};

	inline g_session_t g_session{};
}