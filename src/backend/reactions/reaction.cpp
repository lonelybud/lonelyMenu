#include "reaction.hpp"

#include "backend/player_command.hpp"
#include "core/data/session.hpp"
#include "fiber_pool.hpp"
#include "script.hpp"
#include "services/blocked_players/blocked_players.hpp"
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

	reaction::reaction(reaction_type type, reaction_sub_type sub_type, const char* event_name, bool notify_once, bool is_modder, bool other, reaction_karma karma_type) :
	    type(type),
	    sub_type(sub_type),
	    m_event_name(event_name),
	    notify_once(notify_once),
	    is_modder(is_modder),
	    other(other),
	    m_karma_type(karma_type)
	{
	}

	void reaction::process(player_ptr player, player_ptr target)
	{
		if (player && player->is_valid())
		{
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

			bool kick_player = this->m_karma_type == reaction_karma::kick_player;
			if (this->m_karma_type == reaction_karma::infraction_based && player->infractions[this] > 10)
				kick_player = true;

			// dont log same event in a given time period from the same player. This will keep console logs short and concise.
			bool should_log = true;
			if (player->last_event_id == sub_type && !player->last_event_deb.has_debounced())
				should_log = false;
			else
			{
				player->last_event_id = sub_type;
				player->last_event_deb.reset(1000);
			}
			//

			auto str = std::format("{} from '{}'", m_event_name, player->m_name);
			if (target)
				str += std::format("to {}", target->m_name);
			const char* title = "Event";
			if (this->type == reaction_type::kick_player)
				title = "Received Kick";
			else if (this->type == reaction_type::crash_player)
				title = "Received Crash";

			if (log && should_log)
				LOG(WARNING) << title << ": " << str;
			if (notify)
				g_notification_service.push_warning(title, str);

			// add modder to blocked players temporary list
			if (is_modder)
			{
				player->is_modder = true;

				if (!player->is_blocked)
				{
					// add player again and update infraction message if added already
					player->spam_message = get_infraction_str(player->infractions);
					g_blocked_players_service.add_player(player, false, player->is_spammer);
				}
			}
			else if (other)
				player->is_other = true;

			if (!player->is_friend() && kick_player)
			{
				player->is_pain_in_ass = true;

				// trying to kick you. let give 3 warnings before kicking that bud
				if (this->type == reaction_type::kick_player && g_player_service->get_self()->is_host() && (++player->kick_counts < 4))
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
