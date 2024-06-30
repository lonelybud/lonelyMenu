#include "reaction.hpp"

#include "backend/player_command.hpp"
#include "core/data/session.hpp"
#include "fiber_pool.hpp"
#include "script.hpp"
#include "services/blocked_players/blocked_players.hpp"
#include "services/gui/gui_service.hpp"
#include "services/players/player_service.hpp"
#include "util/player.hpp"

namespace big
{
	reaction::reaction(reaction_type type, reaction_sub_type sub_type, const char* event_name, reaction_notif_type notif_type, player_type plyr_type, reaction_karma karma_type, int attempts_before_log) :
	    m_type(type),
	    m_sub_type(sub_type),
	    m_event_name(event_name),
	    m_notif_type(notif_type),
	    m_plyr_type(plyr_type),
	    m_karma_type(karma_type),
	    m_attempts_before_log(attempts_before_log)
	{
	}

	void reaction::process(player_ptr player, player_ptr target)
	{
		if (player && player->is_valid())
		{
			auto curr_time = std::chrono::system_clock::now();

			/************************************************************ update infraction */

			if (!player->infractions.contains(this))
				player->infractions[this] = 1;
			else
			{
				if (m_notif_type == reaction_notif_type::once)
					return;

				// use these variables to avoid getting crash infraction of same type from multiple players at the same time aka false positives
				// which can result in kicking innocent players
				static reaction_sub_type last_sub_type = reaction_sub_type::none;
				static player_ptr last_sub_type_plyr   = nullptr;
				static std::chrono::system_clock::time_point last_sub_type_time = std::chrono::system_clock::time_point::min();
				constexpr std::chrono::milliseconds time_interval = std::chrono::milliseconds(500);

				if (m_type == reaction_type::crash_player && m_sub_type == last_sub_type && player != last_sub_type_plyr && (curr_time - last_sub_type_time) <= time_interval)
					;
				else
					++player->infractions[this];

				last_sub_type      = m_sub_type;
				last_sub_type_plyr = player;
				last_sub_type_time = curr_time;
				//
			}

			/************************************************************ prevent log spam */
			// dont log same event in a given time period from the same player. This will keep console logs short and concise.
			// example if you receive "request control event" 30 times in 5 secs and you have set not to log more than 1 in 1sec
			// then it will only log 5 times (each after 1 sec) and not 30 times.

			bool should_log = false;

			if (player->last_event_sub_type == m_sub_type)
			{
				if (++player->last_event_count > m_attempts_before_log)
				{
					should_log               = player->last_event_timer.has_time_passed();
					player->last_event_count = 0;
				}
			}
			else
			{
				player->last_event_sub_type = m_sub_type;
				player->last_event_timer.reset(1000);

				if (m_attempts_before_log)
				{
					player->last_event_count = 1;
					should_log               = false;
				}
				else
				{
					player->last_event_count = 0;
					should_log               = true;
				}
			}

			/************************************************************ log phase */

			if (m_notif_type != reaction_notif_type::silent)
			{
				auto str = std::format("{} from '{}'", m_event_name, player->m_name);
				if (target)
					str += std::format("to {}", target->m_name);
				const char* title = "Event";
				if (m_type == reaction_type::kick_player)
					title = "Received Kick";
				else if (m_type == reaction_type::crash_player)
					title = "Received Crash";

				if (log && should_log)
					LOG(WARNING) << title << ": " << str;
				if (notify)
					g_notification_service.push_warning(title, str);
			}

			/************************************************************ whether to add player in temporary list */

			if (m_plyr_type != player_type::normal)
			{
				player->plyr_type = m_plyr_type;

				if (!player->is_blocked)
				{
					// add player to list and update infraction message if added already
					player->spam_message = get_infraction_str(player->infractions);
					g_blocked_players_service.add_player(player, false, player->is_spammer);
				}
			}

			/************************************************************ finally decide to kick player */

			bool kick_player = m_karma_type == reaction_karma::kick_player;
			if (m_karma_type == reaction_karma::infraction_based && player->infractions[this] > 10)
				kick_player = true;

			if (!player->is_friend() && kick_player)
			{
				player->is_pain_in_ass = true; // make him bright red in player list

				// trying to kick you. let give 3 warnings before kicking that bud
				if (m_type == reaction_type::kick_player && g_player_service->get_self()->is_host() && (++player->kick_counts < 4))
					return;

				// block join
				if (!player->is_blocked)
				{
					player->is_blocked   = true;
					player->spam_message = get_infraction_str(player->infractions);
					g_blocked_players_service.add_player(player, true, false);
				}

				// kick when host
				if (g_player_service->get_self()->is_host())
					dynamic_cast<player_command*>(command::get("breakup"_J))->call(player);
			}
		}
		else
			g_notification_service.push_warning("Reaction", std::format("{} from '?'", m_event_name), true);
	}
}
