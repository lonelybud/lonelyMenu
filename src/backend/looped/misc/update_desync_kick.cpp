#include "backend/looped/looped.hpp"
#include "core/data/desync_kick.hpp"
#include "gta_util.hpp"

namespace big
{
	void looped::system_update_desync_kick()
	{
		memset(&gta_util::get_network()->m_game_complaint_mgr.m_host_tokens_complained, 0, 64 * sizeof(uint64_t));
		gta_util::get_network()->m_game_complaint_mgr.m_num_tokens_complained = 0;

		for (auto& pair : g_desync_kick_players)
			if (pair.second->is_valid())
				gta_util::get_network()->m_game_complaint_mgr.raise_complaint(pair.second->m_host_token);

		if (gta_util::get_network()->m_game_session_state > 3 && gta_util::get_network()->m_game_session_state < 6
		    && gta_util::get_network()->m_game_complaint_mgr.m_num_tokens_complained && g_player_service->get_self()->is_valid()
		    && !g_player_service->get_self()->is_host())
			g_pointers->m_gta.m_reset_network_complaints(&gta_util::get_network()->m_game_complaint_mgr);
	}
}