#pragma once
#include "core/data/session.hpp"
#include "gta_util.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "script.hpp"
#include "script_function.hpp"
#include "util/globals.hpp"
#include "util/misc.hpp"

#include <network/CCommunications.hpp>
#include <script/globals/GPBD_FM_3.hpp>

namespace big::session
{
	inline bool join_type(eSessionType session)
	{
		SCRIPT::REQUEST_SCRIPT_WITH_NAME_HASH("pausemenu_multiplayer"_J);

		while (!SCRIPT::HAS_SCRIPT_WITH_NAME_HASH_LOADED("pausemenu_multiplayer"_J))
			script::get_current()->yield();

		if (session == eSessionType::LEAVE_ONLINE)
			*scr_globals::session.at(2).as<int*>() = -1;
		else
		{
			*scr_globals::session.at(2).as<int*>() = 0;
			*scr_globals::session2.as<int*>()      = (int)session;
		}

		*scr_globals::session.as<int*>() = 1;

		if (*g_pointers->m_gta.m_is_session_started && session != eSessionType::LEAVE_ONLINE)
		{
			*scr_globals::transition_state.as<eTransitionState*>() = eTransitionState::TRANSITION_STATE_LOOK_TO_JOIN_ANOTHER_SESSION_FM;
		}
		else if (session == eSessionType::LEAVE_ONLINE)
		{
			*scr_globals::transition_state.as<eTransitionState*>() = eTransitionState::TRANSITION_STATE_RETURN_TO_SINGLEPLAYER;
		}

		scr_functions::reset_session_data({true, true});
		*scr_globals::session3.as<int*>() = 0;
		*scr_globals::session4.as<int*>() = 1;
		*scr_globals::session5.as<int*>() = 32;

		if (SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH("maintransition"_J) == 0)
		{
			*scr_globals::session6.as<int*>() = 1;
			script::get_current()->yield(200ms);
			*scr_globals::session.as<int*>() = 0;
		}

		SCRIPT::SET_SCRIPT_WITH_NAME_HASH_AS_NO_LONGER_NEEDED("pausemenu_multiplayer"_J);
		return true;
	}

	inline void set_fm_event_index(int index)
	{
		int idx = index / 32;
		int bit = index % 32;
		misc::set_bit(scr_globals::gsbd_fm_events.at(11).at(361).at(idx, 1).as<int*>(), bit);
		misc::set_bit(scr_globals::gsbd_fm_events.at(11).at(353).at(idx, 1).as<int*>(), bit);
		misc::set_bit((int*)&scr_globals::gpbd_fm_3.as<GPBD_FM_3*>()->Entries[self::id].BossGoon.ActiveFreemodeEvents[idx], bit);
	}

	inline void clear_fm_event_index(int index)
	{
		int idx = index / 32;
		int bit = index % 32;
		misc::clear_bit(scr_globals::gsbd_fm_events.at(11).at(361).at(idx, 1).as<int*>(), bit);
		misc::clear_bit(scr_globals::gsbd_fm_events.at(11).at(353).at(idx, 1).as<int*>(), bit);
		misc::clear_bit((int*)&scr_globals::gpbd_fm_3.as<GPBD_FM_3*>()->Entries[self::id].BossGoon.ActiveFreemodeEvents[idx], bit);
	}

	inline void set_host_tokens()
	{
		auto token                = *g_pointers->m_gta.m_host_token;
		g_session.orig_host_token = g_session.host_token = token;
		LOGF(VERBOSE, "Your host token is: {}", token);
	}

	inline void change_host_token()
	{
		auto host_token = g_session.host_token;

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

		if (g_local_player->m_player_info)
			g_local_player->m_player_info->m_net_player_data.m_host_token = host_token;
	}

	inline bool is_spoofed_host_token(uint64_t token)
	{
		if (token < 1000'000'000'000)
			return true;

		return false;
	}
}
