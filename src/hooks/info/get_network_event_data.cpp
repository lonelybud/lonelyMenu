#include "core/data/misc.hpp"
#include "core/data/reactions.hpp"
#include "core/data/session.hpp"
#include "fiber_pool.hpp"
#include "gta/net_game_event.hpp"
#include "hooking/hooking.hpp"
#include "services/notifications/notification_service.hpp"
#include "services/players/player_service.hpp"
#include "util/entity.hpp"
#include "util/player.hpp"
#include "util/vehicle.hpp"

namespace big
{
	rage::CEventNetwork* hooks::get_network_event_data(int64_t unk, rage::CEventNetwork* net_event)
	{
		switch (net_event->get_type())
		{
		case rage::eEventNetworkType::CEventNetworkRemovedFromSessionDueToComplaints:
		{
			g_notification_service.push_warning("Kicked", "You have been desync kicked.", true);
			g_session.next_host_list.list.clear();
			break;
		}
		case rage::eEventNetworkType::CEventNetworkEntityDamage:
		{
			rage::sEntityDamagedData damage_data;
			net_event->get_extra_information(&damage_data, sizeof(damage_data));

			if (auto damager = reinterpret_cast<CPed*>(g_pointers->m_gta.m_handle_to_ptr(damage_data.m_damager_index));
			    damager && damager->m_entity_type == 4 && damager->m_player_info && damage_data.m_victim_destroyed)
				if (auto victim = reinterpret_cast<CPed*>(g_pointers->m_gta.m_handle_to_ptr(damage_data.m_victim_index));
				    victim && victim->m_entity_type == 4 && damager != victim)
					if (auto player = g_player_service->get_by_host_token(damager->m_player_info->m_net_player_data.m_host_token))
					{
						player_ptr victim_player;

						if (victim->m_player_info)
						{
							victim_player                 = victim == g_local_player ?
							                    g_player_service->get_self() :
							                    g_player_service->get_by_host_token(victim->m_player_info->m_net_player_data.m_host_token);
							victim_player->last_killed_by = player;
						}
						// no need to run code for killing peds if he already done that
						else if (player->infractions.contains(&g_reactions.killed_ped_with_god)
						    || player->infractions.contains(&g_reactions.killed_ped_with_invis))
							return g_hooking->get_original<get_network_event_data>()(unk, net_event);

						if (victim == g_local_player)
							g_fiber_pool->queue_job([player, headshot = damage_data.m_is_headshot] {
								std::string str = "You got Killed by: " + std::string(player->m_name);

								if (is_player_in_submarine(player->id()))
									str += " (submarine)";

								if (headshot)
									str += " (headshot)";

								LOG(WARNING) << str;

								if (g_local_player->m_vehicle
								    && (!g_local_player->m_vehicle->m_driver || g_local_player->m_vehicle->m_driver == g_local_player))
								{
									auto veh = g_pointers->m_gta.m_ptr_to_handle(g_local_player->m_vehicle);

									if (is_my_spawned_vehicle(veh)) // prevent request control event on someones else spawned vehicles
									{
										if (entity::take_control_of(g_local_player->m_vehicle))
										{
											g_local_player->m_vehicle->m_door_lock_status = (int)eVehicleLockState::VEHICLELOCK_LOCKED;
											g_notification_service.push_success("Vehicle Lock", "Success after you died", true);
										}
										else
											g_notification_service.push_error("Vehicle Lock", "Failed after you died", true);
									}
								}

								if (!player->is_friend())
									player->esp_enemy = true;
							});
						else if (victim_player && g_misc.notify_friend_killed && victim_player->is_friend())
							g_notification_service.push_warning("Friend Killed",
							    std::format("{} killed {}", player->m_name, victim_player->m_name),
							    true);

						if (is_using_orbital_cannon(player))
						{
							if (victim_player)
								g_reactions.killed_with_orbital.process(player, victim_player);
						}
						else if (is_invincible(player))
						{
							if (victim_player)
								g_reactions.killed_with_god.process(player, victim_player);
							else
								g_reactions.killed_ped_with_god.process(player);
						}
						else if (is_invincible_veh(player) && victim_player)
							g_reactions.killed_with_veh_god.process(player, victim_player);

						if (is_player_invisible(player))
						{
							if (victim_player)
								g_reactions.killed_with_invis.process(player, victim_player);
							else
								g_reactions.killed_ped_with_invis.process(player);
						}

						if (is_hidden_from_player_list(player) && victim_player)
							g_reactions.killed_when_hidden.process(player, victim_player);
					}

			break;
		}
		}

		return g_hooking->get_original<get_network_event_data>()(unk, net_event);
	}
}
