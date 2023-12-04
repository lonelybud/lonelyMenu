#pragma once
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
			auto it = std::remove_if(list.begin(), list.end(), [id](auto& p) {
				return p.first == id;
			});

			list.erase(it, list.end());
		}

		void filter_current_host()
		{
			for (const auto& [_, plyr] : g_player_service->players())
				if (plyr->is_host())
				{
					delete_plyr(plyr->id());
					return;
				}
		}
	};

	struct g_session_t
	{
		uint64_t smallest_host_token          = 0;
		std::string smallest_host_token_owner = "";
		next_host_list next_host_list;

		uint64_t orig_host_token = 0;
		uint64_t host_token      = 0;
		bool lock_session        = false;

		bool log_chat_messages_to_textbox = true;
		bool decloak_players              = false;

		struct custom_time
		{
			bool override_time = false;
			int hour{}, minute{}, second{};
		} custom_time;
	};

	inline g_session_t g_session{};
}