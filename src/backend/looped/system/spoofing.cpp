#include "backend/looped/looped.hpp"
#include "core/data/session.hpp"
#include "core/scr_globals.hpp"
#include "gta_util.hpp"
#include "natives.hpp"

#include <network/CCommunications.hpp>
#include <script/globals/GPBD_FM.hpp>
#include <script/globals/GlobalPlayerBD.hpp>

namespace big
{
	static uint64_t host_token = 0;

	void looped::system_spoofing()
	{
		if (!g_session.orig_host_token && *g_pointers->m_gta.m_host_token)
			g_session.orig_host_token = host_token = g_session.host_token = g_session.smallest_host_token =
			    *g_pointers->m_gta.m_host_token;

		if (host_token != g_session.host_token && gta_util::get_network()->m_game_session_state == 0)
		{
			host_token = g_session.host_token;

			LOG(VERBOSE) << "Last host token: " << *g_pointers->m_gta.m_host_token;

			if (host_token == g_session.orig_host_token)
				LOG(VERBOSE) << "Using original host token: " << host_token;
			else
				LOG(VERBOSE) << "Using custom host token: " << host_token;

			*g_pointers->m_gta.m_host_token = host_token;

			if (gta_util::get_network()->m_game_session_ptr)
				gta_util::get_network()->m_game_session_ptr->m_local_player.m_player_data.m_host_token = host_token;

			g_pointers->m_gta.m_profile_gamer_info->m_host_token                                       = host_token;
			g_pointers->m_gta.m_player_info_gamer_info->m_host_token                                   = host_token;
			(*g_pointers->m_gta.m_communications)->m_voice.m_connections[0]->m_gamer_info.m_host_token = host_token;

			if (g_local_player && g_local_player->m_player_info)
				g_local_player->m_player_info->m_net_player_data.m_host_token = host_token;
		}
	}
}
