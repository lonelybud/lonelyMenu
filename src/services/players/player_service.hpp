#pragma once
#include "player.hpp"

#include <network/CNetGamePlayer.hpp>

namespace big
{
	class player;

	using player_ptr   = std::shared_ptr<player>;
	using player_entry = std::pair<std::string, player_ptr>;
	using players      = std::multimap<std::string, player_ptr>;

	class player_service final
	{
		CNetGamePlayer** m_self;

		player_ptr m_self_ptr;

		players m_players;

		player_ptr m_dummy = std::make_shared<player>(nullptr, 0);
		player_ptr m_selected_player;

	public:
		player_service();
		~player_service();

		player_service(const player_service&)                = delete;
		player_service(player_service&&) noexcept            = delete;
		player_service& operator=(const player_service&)     = delete;
		player_service& operator=(player_service&&) noexcept = delete;

		void do_cleanup();

		[[nodiscard]] player_ptr get_self();

		[[nodiscard]] player_ptr get_by_id(uint32_t id) const;
		[[nodiscard]] player_ptr get_by_host_token(uint64_t token) const;
		[[nodiscard]] player_ptr get_selected() const;

		player_ptr player_join(CNetGamePlayer* net_game_player, uint64_t host_token);
		void player_leave(CNetGamePlayer* net_game_player);

		players& players()
		{
			return m_players;
		}

		void iterate(const std::function<void(const player_entry& entry)> func)
		{
			for (const auto& iter : m_players)
				func(iter);
		}

		void set_selected(player_ptr plyr);

		std::optional<player_ptr> m_player_to_use_end_session_kick = std::nullopt;
		std::optional<player_ptr> m_player_to_use_complaint_kick   = std::nullopt;
	};

	inline player_service* g_player_service{};
}
