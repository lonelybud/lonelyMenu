#include "player_service.hpp"

#include "core/data/desync_kick.hpp"
#include "core/data/gui_info.hpp"
#include "gta_util.hpp"

#include <network/CNetworkPlayerMgr.hpp>

namespace big
{
	player_service::player_service() :
	    m_self(nullptr)
	{
		const auto network_player_mgr = gta_util::get_network_player_mgr();

		g_player_service = this;
		m_self           = &network_player_mgr->m_local_net_player;
	}

	player_service::~player_service()
	{
		g_player_service = nullptr;
	}

	void player_service::do_cleanup()
	{
		m_selected_player = nullptr;
		m_players.clear();
		m_players_sending_modder_beacons.clear();
		g_gui_info.update_gui_info();
	}

	player_ptr player_service::get_by_id(uint32_t id) const
	{
		if (auto it = m_players.find(id); it != m_players.end())
			return it->second;
		return nullptr;
	}

	player_ptr player_service::get_by_host_token(uint64_t token) const
	{
		for (const auto& [_, player] : m_players)
			if (player && player->m_host_token == token)
				return player;
		return nullptr;
	}

	player_ptr player_service::get_by_name(char* name) const
	{
		for (const auto& [_, player] : m_players)
			if (player && strcmp(player->m_name, name) == 0)
				return player;
		return nullptr;
	}

	player_ptr player_service::get_selected() const
	{
		return m_selected_player;
	}

	player_ptr player_service::get_self()
	{
		if (!m_self_ptr || !m_self_ptr->equals(*m_self))
		{
			m_self_ptr = std::make_shared<player>(*m_self, 0);
			LOG(VERBOSE) << "Created self player";
		}

		return m_self_ptr;
	}

	void player_service::player_join(CNetGamePlayer* net_game_player, uint64_t host_token)
	{
		if (net_game_player == nullptr)
		{
			LOG(WARNING) << "player_join: net_game_player is nullptr";
			return;
		}

		auto plyr = std::make_shared<player>(net_game_player, host_token);

		const auto [pair, inserted] = m_players.insert({plyr->id(), std::move(plyr)});
		if (!inserted)
			LOG(WARNING) << plyr->id() << " alreay exists. Not adding new player - " << net_game_player->get_name();

		g_gui_info.update_gui_info();
	}

	void player_service::player_leave(CNetGamePlayer* net_game_player, rock_id rockstar_id)
	{
		if (net_game_player == nullptr)
		{
			LOG(WARNING) << "player_leave: net_game_player is nullptr for " << rockstar_id;
			return;
		}

		if (m_selected_player && m_selected_player->equals(net_game_player))
			m_selected_player = nullptr;

		if (auto it = m_players.find(net_game_player->m_player_id); it != m_players.end())
		{
			g_desync_kick_players.erase(it->second->m_rockstar_id);
			m_players.erase(it);
		}

		g_gui_info.update_gui_info();
	}

	void player_service::mark_player_as_sending_modder_beacons(std::uint64_t rid)
	{
		m_players_sending_modder_beacons.insert(rid);
	}

	bool player_service::did_player_send_modder_beacon(std::uint64_t rid)
	{
		return m_players_sending_modder_beacons.contains(rid);
	}

	void player_service::set_selected(player_ptr plyr)
	{
		m_selected_player = plyr;
	}
}
