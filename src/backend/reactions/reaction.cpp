#include "reaction.hpp"

#include "backend/player_command.hpp"
#include "core/data/infractions.hpp"
#include "core/data/session.hpp"
#include "fiber_pool.hpp"
#include "script.hpp"
#include "services/bad_players/bad_players.hpp"
#include "services/gui/gui_service.hpp"
#include "services/players/player_service.hpp"
#include "util/notify.hpp"

namespace big
{
	reaction::reaction(int id, const char* event_name, const char* notify_message) :
	    m_id(id),
	    m_event_name(event_name),
	    m_notify_message(notify_message)
	{
	}

	void reaction::process(player_ptr player, bool kick_player, Infraction infraction, bool is_modder, bool is_toxic)
	{
		rage::rlGamerInfo* net_data;

		if (player && player->is_valid() && (net_data = player->get_net_data()))
		{
			auto rockstar_id = net_data->m_gamer_handle.m_rockstar_id;
			auto name        = net_data->m_name;

			if (infraction != Infraction::NONE)
			{
				if (!player->infractions.contains((int)infraction))
					player->infractions.insert((int)infraction);
				// infraction must have been logged/notified before so return. Exception - crash & kick
				else if (infraction != Infraction::TRIED_CRASH_PLAYER && infraction != Infraction::TRIED_KICK_PLAYER)
					return;
			}

			if (infraction == Infraction::TRIED_CRASH_PLAYER)
			{
				++player->crash_count;

				if (player->crash_count > 20)
					kick_player = true;
			}

			auto str = std::vformat(m_notify_message, std::make_format_args(name));

			bool should_log = true;
			if (player->last_event_id == m_id)
			{
				if (player->last_event_count >= 5)
					should_log = false;
				else
					++player->last_event_count;
			}
			else
			{
				player->last_event_id    = m_id;
				player->last_event_count = 1;
			}

			if (log && should_log)
				LOG(WARNING) << str;

			if (notify)
				g_notification_service->push_warning("Protections", str);

			if (is_modder)
			{
				player->is_modder = true;

				if (!bad_players_nm::does_exist(rockstar_id))
					bad_players_nm::add_player({name, rockstar_id, false, player->is_spammer});
			}
			else if (is_toxic)
				player->is_toxic = true;

			// open player info of attacker
			if (infraction == Infraction::TRIED_CRASH_PLAYER || infraction == Infraction::TRIED_KICK_PLAYER)
			{
				g_gui_service->set_selected(tabs::PLAYER);
				g_player_service->set_selected(player);
			}

			if (kick_player)
			{
				// player->timeout();

				// block join
				if (!player->is_blocked)
				{
					player->is_blocked = true;
					bad_players_nm::add_player({name, rockstar_id, true, player->is_spammer});
				}

				if (g_player_service->get_self()->is_host())
					dynamic_cast<player_command*>(command::get(RAGE_JOAAT("hostkick")))->call(player);
			}
		}
	}
}
