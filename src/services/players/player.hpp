#pragma once
#include "core/data/player.hpp"
#include "core/data/reactions.hpp"
#include "core/enums.hpp"
#include "rate_limiter.hpp"

#include <unordered_set>

class CVehicle;
class CPed;
class CNetGamePlayer;
class CPlayerInfo;

namespace rage
{
	class snPlayer;
	class snPeer;
	class rlGamerInfo;
	class netConnectionPeer;
}

namespace big
{
	class player_service;

	class player final
	{
		friend class player_service;

		CNetGamePlayer* m_net_game_player = nullptr;
		bool m_is_friend;

	public:
		explicit player(CNetGamePlayer* net_game_player, uint64_t host_token);
		~player() = default;

		player(const player&)                = default;
		player(player&&) noexcept            = default;
		player& operator=(const player&)     = default;
		player& operator=(player&&) noexcept = default;

		[[nodiscard]] CVehicle* get_current_vehicle() const;
		[[nodiscard]] rage::rlGamerInfo* get_net_data() const;
		[[nodiscard]] CNetGamePlayer* get_net_game_player() const;
		[[nodiscard]] CPed* get_ped() const;
		[[nodiscard]] CPlayerInfo* get_player_info() const;
		[[nodiscard]] class rage::snPlayer* get_session_player();
		[[nodiscard]] class rage::snPeer* get_session_peer();
		[[nodiscard]] class rage::netConnectionPeer* get_connection_peer();
		[[nodiscard]] std::optional<netAddress> get_ip_address();
		[[nodiscard]] uint16_t get_port();

		[[nodiscard]] uint8_t id() const;

		[[nodiscard]] bool is_friend() const;
		[[nodiscard]] bool is_host() const;
		[[nodiscard]] bool is_valid() const;

		void timeout();

		uint64_t m_host_token;
		char m_name[20] = "";
		int64_t m_rockstar_id;

		std::map<reaction*, int> infractions;
		std::string spam_message  = "";
		player_ptr last_killed_by = nullptr;

		bool is_modder         = false;
		bool is_blocked        = false;
		bool is_spammer        = false;
		bool whitelist_spammer = false;
		bool whitelist_ptfx    = false;
		bool is_other          = false;
		bool is_known_player   = false;
		bool is_pain_in_ass    = false;
		bool esp_enemy         = false;

		bool block_radio_requests     = false;
		bool block_host_migr_requests = false;

		bool timed_out          = false;
		bool block_explosions   = false;
		bool block_clone_create = false;
		bool block_clone_sync   = false;
		bool block_net_events   = false;

		rate_limiter m_host_migration_rate_limit{2s, 15};
		rate_limiter m_play_sound_rate_limit{1s, 10};
		rate_limiter m_invites_rate_limit{10s, 2};
		rate_limiter m_invites_rate_limit_tse{5s, 2};
		rate_limiter m_radio_request_rate_limit{5s, 2};
		rate_limiter m_ptfx_ratelimit{1s, 5};

		std::chrono::system_clock::time_point last_msg_time = std::chrono::system_clock::time_point::min();
		std::chrono::seconds last_spam_interval_diff{};
		int same_interval_spam_count_low{};
		int same_interval_spam_count_high{};

		bool log_clones = false;

		// prevent spam logging of events in console
		reaction_sub_type last_event_id = reaction_sub_type::none;
		int last_event_count            = 1;

		int spectating_player = -1;

	protected:
		bool equals(const CNetGamePlayer* net_game_player) const;
	};
}