#include "reaction.hpp"

#include "backend/player_command.hpp"
#include "core/data/session.hpp"
#include "fiber_pool.hpp"
#include "script.hpp"
#include "services/bad_players/bad_players.hpp"
#include "services/gui/gui_service.hpp"
#include "services/players/player_service.hpp"

namespace big
{
	inline std::string get_infraction_str(std::map<big::reaction*, int>& infractions)
	{
		std::string str = "infrac: ";

		for (auto& pair : infractions)
			str += std::string(pair.first->m_event_name) + ",";

		return str;
	}

	reaction::reaction(reaction_type type, reaction_sub_type sub_type, const char* event_name, const char* notify_message, bool notify_once, bool is_modder, bool other, int n_events_at_time) :
	    type(type),
	    sub_type(sub_type),
	    m_event_name(event_name),
	    m_notify_message(notify_message),
	    notify_once(notify_once),
	    is_modder(is_modder),
	    other(other),
	    n_events_at_time(n_events_at_time)
	{
	}

	void reaction::process(player_ptr player, player_ptr target)
	{
		if (player && player->is_valid())
		{
			bool kick_player = this->type == reaction_type::kick_player;

			if (!player->infractions.contains(this))
				player->infractions[this] = 1;
			else
			{
				if (this->notify_once)
					return;

				// use these variables to avoid getting crash infraction of same type from multiple players at the same time aka false positives
				// which can result in kicking innocent players
				static reaction_sub_type last_sub_type = reaction_sub_type::none;
				static player_ptr last_sub_type_plyr   = nullptr;
				static std::chrono::system_clock::time_point last_sub_type_time = std::chrono::system_clock::time_point::min();

				auto currentTime = std::chrono::system_clock::now();
				if (this->type == reaction_type::crash_player && this->sub_type == last_sub_type && player != last_sub_type_plyr
				    && std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - last_sub_type_time).count() <= 500)
					;
				else
					++player->infractions[this];

				last_sub_type      = this->sub_type;
				last_sub_type_plyr = player;
				last_sub_type_time = currentTime;
				//
			}

			// auto-kick crashing player
			if (this->type == reaction_type::crash_player)
				switch (this->sub_type)
				{
				case reaction_sub_type::crash1:
				case reaction_sub_type::crash3:
				case reaction_sub_type::crash19:
				case reaction_sub_type::crash21:
				case reaction_sub_type::crash22:
				case reaction_sub_type::scripted_event_crash:
				case reaction_sub_type::stand_user_crash:
				case reaction_sub_type::crash12:
				case reaction_sub_type::crash13:
				case reaction_sub_type::crash39:
				case reaction_sub_type::crash40:
				case reaction_sub_type::player_ped_removal:
				case reaction_sub_type::invalid_weapon_type:
				{
					kick_player = true;
					break;
				}
				case reaction_sub_type::crash14:
				case reaction_sub_type::crash15:
				case reaction_sub_type::crash23:
				case reaction_sub_type::tse_sender_mismatch:
				case reaction_sub_type::crash11:
				case reaction_sub_type::crash8:
				{
					break;
				}
				default:
				{
					if (player->infractions[this] > 10)
						kick_player = true;
					break;
				}
				}

			auto str = std::format("{} from '{}'", m_notify_message, player->m_name);

			if (target)
				str += std::format("to {}", target->m_name);

			// dont log same event more than n time from the same player. This will keep console logs short and concise.
			bool should_log = true;
			if (player->last_event_id == sub_type)
			{
				if (player->last_event_count >= this->n_events_at_time)
					should_log = false;
				else
					++player->last_event_count;
			}
			else
			{
				player->last_event_id    = sub_type;
				player->last_event_count = 1;
			}
			//

			const char* title = nullptr;
			switch (this->type)
			{
			case reaction_type::modder_detection: title = "Modder Detection"; break;
			case reaction_type::kick_player: title = "Received Kick"; break;
			case reaction_type::crash_player: title = "Received Crash"; break;
			default: title = "Event"; break;
			}

			if (log && should_log)
				LOG(WARNING) << title << ": " << str;
			if (notify)
				g_notification_service.push_warning(title, str);

			// add modder to bad players temporary list
			if (is_modder)
			{
				player->is_modder = true;

				if (!g_bad_players_service.does_exist(player->m_rockstar_id))
				{
					player->spam_message = get_infraction_str(player->infractions);
					g_bad_players_service.add_player(player, false, player->is_spammer);
				}
			}
			else if (other)
				player->is_other = true;

			// open player info of crazy dude
			// if (infraction == Infraction::TRIED_CRASH_PLAYER || infraction == Infraction::TRIED_KICK_PLAYER)
			// {
			// 	g_gui_service->set_selected(tabs::PLAYER);
			// 	g_player_service->set_selected(player);
			// }

			if (!player->is_friend() && kick_player)
			{
				player->is_pain_in_ass = true;

				// player->timeout();

				// block join
				if (!player->is_blocked)
				{
					player->is_blocked   = true;
					player->spam_message = get_infraction_str(player->infractions);
					g_bad_players_service.add_player(player, true, false);
				}

				if (g_player_service->get_self()->is_host())
					dynamic_cast<player_command*>(command::get("breakup"_J))->call(player);
			}
		}
		else
			g_notification_service.push_warning("Reaction", std::format("{} from '?'", m_notify_message), true);
	}
}
