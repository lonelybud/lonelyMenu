#include "reaction.hpp"

#include "backend/player_command.hpp"
#include "core/data/infractions.hpp"
#include "core/data/player.hpp"
#include "core/settings/session.hpp"
#include "fiber_pool.hpp"
#include "gui.hpp"
#include "script.hpp"
#include "services/gui/gui_service.hpp"
#include "services/recent_modders/recent_modders.hpp"
#include "util/notify.hpp"

namespace big
{
	reaction::reaction(const char* event_name, const char* notify_message) :
	    m_event_name(event_name),
	    m_notify_message(notify_message)
	{
	}

	void reaction::process(player_ptr player, bool kick_player, Infraction infraction, bool is_modder, bool is_toxic)
	{
		if (player->is_valid())
		{
			if (infraction != Infraction::NONE)
			{
				if (!player->infractions.contains((int)infraction))
					player->infractions.insert((int)infraction);
				// infraction must have been logged/notified before so return. Exception - crash & kick
				else if (infraction != Infraction::TRIED_CRASH_PLAYER && infraction != Infraction::TRIED_KICK_PLAYER)
					return;
			}

			// open player info of attacker
			if ((!player->ignore_crash && infraction == Infraction::TRIED_CRASH_PLAYER) || infraction == Infraction::TRIED_KICK_PLAYER)
			{
				g_gui_service->set_selected(tabs::PLAYER);
				g_player_service->set_selected(player);
				g_gui->open_on_next_tick = true;
			}

			if (is_modder)
				player->is_modder = true;
			if (is_toxic)
				player->is_toxic = true;

			if (log)
				if (auto net_data = player->get_net_data())
					LOG(WARNING) << std::format("Received {} from {} ({})",
					    m_event_name,
					    net_data->m_name,
					    net_data->m_gamer_handle.m_rockstar_id);

			if (notify)
			{
				char notification[500]{}; // I don't like using sprintf but there isn't an alternative afaik
				snprintf(notification, sizeof(notification), m_notify_message, player->get_name());
				g_notification_service->push_warning("Protections", notification);
			}

			if (kick_player)
				if (auto net_data = player->get_net_data())
				{
					auto rockstar_id = net_data->m_gamer_handle.m_rockstar_id;
					auto name        = net_data->m_name;

					player->timeout();

					// block join
					if (!recent_modders_nm::does_exist(rockstar_id))
						recent_modders_nm::add_player({name, rockstar_id, true, player->is_spammer});

					if (g_player_service->get_self()->is_host())
					{
						dynamic_cast<player_command*>(command::get(RAGE_JOAAT("hostkick")))->call(player, {});
						return;
					}
					else if (!is_modder)
					{
						dynamic_cast<player_command*>(command::get(RAGE_JOAAT("desync")))->call(player, {});
						return;
					}
					// become host
					else
						g_fiber_pool->queue_job([player] {
							if (g_session.force_session_host && g_player.host_to_auto_kick != nullptr
							    && g_player.host_to_auto_kick->is_valid()
							    && g_player.host_to_auto_kick != g_player_service->get_self())
							{
								// kick current host
								dynamic_cast<player_command*>(command::get(RAGE_JOAAT("oomkick")))->call(g_player.host_to_auto_kick, {});

								// try to host kick attacker if possible
								for (int i = 0; i < 30; i++)
								{
									script::get_current()->yield(100ms);
									if (g_player_service->get_self()->is_host())
									{
										dynamic_cast<player_command*>(command::get(RAGE_JOAAT("hostkick")))->call(player, {});
										return;
									}
								}
							}
						});

					auto str = std::format("Kicking player {}", net_data->m_name);
					LOG(WARNING) << str;
					g_notification_service->push_warning("Protections", str);
				}
		}
	}
}
