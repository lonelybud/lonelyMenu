#include "player.hpp"

#include "gta_util.hpp"
#include "services/friends/friends_service.hpp"

#include <network/RemoteGamerInfoMsg.hpp>

namespace big
{
	player::player(CNetGamePlayer* net_game_player, uint64_t host_token) :
	    m_net_game_player(net_game_player),
	    m_host_token(host_token)
	{
		if (net_game_player)
		{
			strcpy(m_name, net_game_player->get_name());
			m_rockstar_id = net_game_player->get_net_data()->m_gamer_handle.m_rockstar_id;
			m_is_friend   = friends_service::is_friend(m_rockstar_id);
		}
	}

	CVehicle* player::get_current_vehicle() const
	{
		if (const auto ped = get_ped())
			if (const auto vehicle = ped->m_vehicle; vehicle != nullptr)
				return vehicle;
		return nullptr;
	}

	rage::rlGamerInfo* player::get_net_data() const
	{
		return get_net_game_player() == nullptr ? nullptr : m_net_game_player->get_net_data();
	}

	CNetGamePlayer* player::get_net_game_player() const
	{
		return (m_net_game_player == nullptr || m_net_game_player->m_player_info == nullptr) ? nullptr : m_net_game_player;
	}

	CPed* player::get_ped() const
	{
		if (auto player_info = get_player_info())
			if (auto ped = player_info->m_ped)
				return ped;
		return nullptr;
	}

	CPlayerInfo* player::get_player_info() const
	{
		if (auto net_player = get_net_game_player())
			return net_player->m_player_info;
		return nullptr;
	}

	rage::snPlayer* player::get_session_player()
	{
		for (uint32_t i = 0; i < gta_util::get_network()->m_game_session_ptr->m_player_count; i++)
		{
			if (gta_util::get_network()->m_game_session_ptr->m_players[i]->m_player_data.m_host_token == this->m_host_token)
			{
				return gta_util::get_network()->m_game_session_ptr->m_players[i];
			}
		}

		if (gta_util::get_network()->m_game_session_ptr->m_local_player.m_player_data.m_host_token == this->m_host_token)
			return &gta_util::get_network()->m_game_session_ptr->m_local_player;

		return nullptr;
	}

	rage::snPeer* player::get_session_peer()
	{
		for (uint32_t i = 0; i < gta_util::get_network()->m_game_session_ptr->m_peer_count; i++)
			if (gta_util::get_network()->m_game_session_ptr->m_peers[i]->m_peer_data.m_gamer_handle.m_rockstar_id == m_rockstar_id)
				return gta_util::get_network()->m_game_session_ptr->m_peers[i];

		return nullptr;
	}

	rage::netConnectionPeer* player::get_connection_peer()
	{
		if (auto session_player = get_session_player())
			if (auto peer = g_pointers->m_gta.m_get_connection_peer(gta_util::get_network()->m_game_session_ptr->m_net_connection_mgr,
			        (int)get_session_player()->m_player_data.m_peer_id_2))
				return peer;

		return nullptr;
	}

	std::optional<netAddress> player::get_ip_address()
	{
		if (auto peer = get_connection_peer())
		{
			if (peer->m_peer_address.m_connection_type != 1)
				return std::nullopt;

			return peer->m_peer_address.m_external_ip;
		}

		return std::nullopt;
	}

	uint16_t player::get_port()
	{
		if (auto peer = get_connection_peer())
		{
			if (peer->m_peer_address.m_connection_type != 1)
				return 0;

			return peer->m_peer_address.m_external_port;
		}

		return 0;
	}

	uint8_t player::id() const
	{
		return get_net_game_player() == nullptr ? -1 : m_net_game_player->m_player_id;
	}

	bool player::is_host() const
	{
		return get_net_game_player() == nullptr ? false : m_net_game_player->is_host();
	}

	bool player::is_friend() const
	{
		return m_is_friend || is_trusted;
	}

	bool player::is_valid() const
	{
		return get_net_game_player() == nullptr ? false : m_net_game_player->is_valid();
	}

	bool player::equals(const CNetGamePlayer* net_game_player) const
	{
		return net_game_player == m_net_game_player;
	}

	void player::timeout()
	{
		this->block_net_events   = this->timed_out;
		this->block_clone_sync   = this->timed_out;
		this->block_clone_create = this->timed_out;
		this->block_explosions   = this->timed_out;

		LOGF(WARNING, "Timeout player '{}' : {}", this->m_name, this->timed_out ? "true" : "false");
	}
}
