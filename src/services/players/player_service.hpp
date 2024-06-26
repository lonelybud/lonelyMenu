#pragma once
#include "player.hpp"

#include <network/CNetGamePlayer.hpp>

namespace big
{
	using player_entry = std::pair<uint8_t, player_ptr>;
	using players      = std::map<uint8_t, player_ptr>;

	class player_service final
	{
		CNetGamePlayer** m_self;

		player_ptr m_self_ptr;

		players m_players;

		// player_ptr m_dummy = std::make_shared<player>(nullptr, 0);
		player_ptr m_selected_player;

		std::unordered_set<std::uint64_t> m_players_sending_modder_beacons;

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
		[[nodiscard]] player_ptr get_by_name(char* name) const;
		[[nodiscard]] player_ptr get_selected() const;

		void player_join(CNetGamePlayer* net_game_player, uint64_t host_token);
		void player_leave(CNetGamePlayer* net_game_player, rock_id rockstar_id);
		void mark_player_as_sending_modder_beacons(std::uint64_t rid);
		bool did_player_send_modder_beacon(std::uint64_t rid);

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
	};

	inline player_service* g_player_service{};
}
