#include "core/data/debug.hpp"
#include "core/data/misc.hpp"
#include "core/data/protections.hpp"
#include "core/data/reactions.hpp"
#include "core/scr_globals.hpp"
#include "gta/net_game_event.hpp"
#include "gta_util.hpp"
#include "hooking/hooking.hpp"
#include "natives.hpp"
#include "util/math.hpp"
#include "util/player.hpp"
#include "util/protection.hpp"

#include <network/CNetGamePlayer.hpp>
#include <script/globals/GPBD_FM_3.hpp>
#include <script/globals/GlobalPlayerBD.hpp>
#include <script/CGameScriptHandlerNetComponent.hpp>

namespace big
{
	bool hooks::scripted_game_event(CScriptedGameEvent* scripted_game_event, CNetGamePlayer* player)
	{
		const auto args       = scripted_game_event->m_args;
		const auto args_count = scripted_game_event->m_args_size / 8;
		const auto hash       = static_cast<eRemoteEvent>(args[0]);
		auto plyr             = g_player_service->get_by_id(player->m_player_id);

		if (!plyr)
		{
			LOG(WARNING) << "scripted_game_event player not found: " << player->get_name();
			return true;
		}

		switch (hash)
		{
		case eRemoteEvent::Bounty:
			if (args[3] == self::id)
				g_reactions.bounty.process(plyr);
			break;
		case eRemoteEvent::CeoKick:
			if (!is_player_our_boss(player->m_player_id))
			{
				g_reactions.ceo_kick.process(plyr);
				return true;
			}
			break;
		case eRemoteEvent::CeoMoney:
			if (g_protections.script_events.ceo_money && !is_player_our_boss(player->m_player_id))
			{
				g_reactions.ceo_money.process(plyr);
				return true;
			}
			break;
		case eRemoteEvent::ClearWantedLevel:
			if (!plyr->is_friend() && g_protections.script_events.clear_wanted_level
			    && !is_in_vehicle(plyr->get_ped(), g_local_player->m_vehicle))
			{
				g_reactions.clear_wanted_level.process(plyr);
				if (g_block_reaction)
					return true;
			}
			break;
		case eRemoteEvent::Crash: g_reactions.scripted_event_crash.process(plyr); return true;
		case eRemoteEvent::Crash2:
			if (args[3] > 32) // actual crash condition is if args[2] is above 255
			{
				g_reactions.scripted_event_crash.process(plyr);
				return true;
			}
			break;
		case eRemoteEvent::Crash3:
		{
			if (isnan(*(float*)&args[3]) || isnan(*(float*)&args[4]) || isnan(*(float*)&args[5]))
			{
				g_reactions.scripted_event_crash.process(plyr);
				return true;
			}

			if (args[3] == -4640169)
			{
				g_log.log_additional(std::format("args[7] = {},  args[8] = {}", args[7], args[8]));

				// can result in false postives but i want to see if that happens
				g_reactions.elegant_crash.process(plyr);
				return true;
			}

			break;
		}
		case eRemoteEvent::Notification:
		{
			switch (static_cast<eRemoteEvent>(args[3]))
			{
			case eRemoteEvent::NotificationMoneyBanked: // never used
			case eRemoteEvent::NotificationMoneyRemoved:
			case eRemoteEvent::NotificationMoneyStolen: g_reactions.fake_deposit.process(plyr); return true;
			case eRemoteEvent::NotificationCrash1:          // this isn't used by the game
				g_reactions.stand_user_crash.process(plyr); // stand user detected
				return true;
			case eRemoteEvent::NotificationCrash2:
				if (!gta_util::find_script_thread("gb_salvage"_J))
				{
					// This looks like it's meant to trigger a sound crash by spamming too many notifications. We've already patched it, but the notifications are still annoying
					g_reactions.stand_user_crash.process(plyr); // stand user detected
					return true;
				}
				break;
			}

			break;
		}
		case eRemoteEvent::ForceMission:
			if (g_protections.script_events.force_mission)
			{
				g_reactions.force_mission.process(plyr);
				return true;
			}
			break;
		case eRemoteEvent::GiveCollectible:
			if (g_protections.script_events.give_collectible)
			{
				g_reactions.give_collectible.process(plyr);
				return true;
			}
			break;
		case eRemoteEvent::GtaBanner:
			if (g_protections.script_events.gta_banner)
			{
				g_reactions.gta_banner.process(plyr);
				return true;
			}
			break;
		case eRemoteEvent::MCTeleport:
			if (!plyr->is_friend() && g_protections.script_events.mc_teleport && args[4] <= 32
			    && !is_player_our_boss(player->m_player_id))
			{
				for (int i = 0; i < 32; i++)
				{
					if (args[5 + i] == NETWORK::NETWORK_HASH_FROM_PLAYER_HANDLE(self::id))
					{
						g_reactions.mc_teleport.process(plyr);
						if (g_block_reaction)
							return true;
					}
				}
			}
			else if (args[4] > 32)
			{
				g_reactions.scripted_event_crash.process(plyr);
				return true;
			}
			break;
		case eRemoteEvent::PersonalVehicleDestroyed:
			if (g_protections.script_events.personal_vehicle_destroyed)
			{
				g_reactions.personal_vehicle_destroyed.process(plyr);
				return true;
			}
			break;
		case eRemoteEvent::RemoteOffradar:
			if (!plyr->is_friend() && g_protections.script_events.remote_off_radar && !is_player_our_boss(player->m_player_id) && !player_is_driver(plyr))
			{
				g_reactions.remote_off_radar.process(plyr);
				if (g_block_reaction)
					return true;
			}
			break;
		case eRemoteEvent::TSECommand:
		{
			if (NETWORK::NETWORK_IS_ACTIVITY_SESSION())
				break;

			if (g_protections.script_events.rotate_cam && static_cast<eRemoteEvent>(args[3]) == eRemoteEvent::TSECommandRotateCam)
			{
				g_reactions.rotate_cam.process(plyr);
				return true;
			}

			if (g_protections.script_events.sound_spam && static_cast<eRemoteEvent>(args[3]) == eRemoteEvent::TSECommandSound)
			{
				if (g_misc.log_sound_event)
					g_dbg_sound_event.process(plyr, nullptr, "(TSECommand)");

				if (plyr->m_play_sound_rate_limit_tse.process())
				{
					g_reactions.sound_spam_tse_rate_limit.process(plyr);
					return true;
				}
			}

			break;
		}
		case eRemoteEvent::SendToCayoPerico:
			if (g_protections.script_events.send_to_location && args[4] == 0)
			{
				g_reactions.send_to_location.process(plyr);
				return true;
			}
			break;
		case eRemoteEvent::SendToCutscene:
			if (g_protections.script_events.send_to_cutscene && !is_player_our_boss(player->m_player_id))
			{
				g_reactions.send_to_cutscene.process(plyr);
				return true;
			}
			else
				g_notification_service.push_warning("SendToCutscene", std::format("Event ack to {}", plyr->m_name), true);
			break;
		case eRemoteEvent::SendToLocation:
		{
			if (is_player_our_boss(player->m_player_id))
				break;

			bool known_location = false;

			if (args[3] == 0 && args[4] == 0)
			{
				if (args[5] == 4 && args[6] == 0)
				{
					known_location = true;

					if (g_protections.script_events.send_to_location)
					{
						g_reactions.send_to_location.process(plyr);
						return true;
					}
				}
				else if ((args[5] == 3 || args[5] == 4) && args[6] == 1)
				{
					known_location = true;

					if (g_protections.script_events.send_to_location)
					{
						g_reactions.send_to_location.process(plyr);
						return true;
					}
				}
			}

			if (!known_location)
			{
				g_reactions.tse_freeze.process(plyr);
				return true;
			}
			break;
		}
		case eRemoteEvent::SoundSpam:
		{
			if (g_misc.log_sound_event)
				g_dbg_sound_event.process(plyr, nullptr, "(invite)");

			if (g_protections.script_events.sound_spam)
			{
				if (plyr->m_invites_rate_limit.process())
				{
					g_reactions.sound_spam_invite_rate_limit.process(plyr);
					return true;
				}
			}
			break;
		}
		case eRemoteEvent::Spectate:
			if (g_protections.script_events.spectate)
			{
				g_reactions.spectate_notification.process(plyr);
				return true;
			}
			break;
		case eRemoteEvent::Teleport:
			if (!plyr->is_friend() && g_protections.script_events.force_teleport)
			{
				if (is_player_same_team(player->m_player_id))
					g_log.log_additional(std::format("Force Teleport 1: {}", plyr->m_name));
				else if (!player_is_driver(plyr))
				{
					g_reactions.force_teleport.process(plyr);
					if (g_block_reaction)
						return true;
				}
				else
					g_log.log_additional(std::format("Force Teleport 2: {}", plyr->m_name));
			}
			break;
		case eRemoteEvent::TransactionError: g_reactions.transaction_error.process(plyr); return true;
		case eRemoteEvent::VehicleKick:
			if (!plyr->is_friend() && g_protections.script_events.vehicle_kick)
			{
				auto val = player_is_driver(plyr, true);
				if (val == 1)
				{
					g_reactions.vehicle_kick_prob.process(plyr);
					if (g_block_reaction)
						return true;
				}
				else if (val == 0)
				{
					g_reactions.vehicle_kick.process(plyr);
					if (g_block_reaction)
						return true;
				}
			}
			break;
		case eRemoteEvent::NetworkBail: g_reactions.network_bail.process(plyr); return true;
		case eRemoteEvent::TeleportToWarehouse:
			if (!plyr->is_friend() && g_protections.script_events.teleport_to_warehouse && !player_is_driver(plyr))
			{
				g_reactions.teleport_to_warehouse.process(plyr);
				return true;
			}
			break;
		case eRemoteEvent::StartActivity:
		{
			eActivityType activity = static_cast<eActivityType>(args[3]);
			if (g_protections.script_events.start_activity)
			{
				if (activity == eActivityType::Survival || activity == eActivityType::Mission || activity == eActivityType::Deathmatch || activity == eActivityType::BaseJump || activity == eActivityType::Race)
				{
					g_reactions.tse_freeze.process(plyr);
					return true;
				}
				else if (activity == eActivityType::Darts)
				{
					g_reactions.start_activity.process(plyr);
					return true;
				}
				else if (activity == eActivityType::PilotSchool)
				{
					g_reactions.start_activity.process(plyr);
					return true;
				}
				else if (activity == eActivityType::ImpromptuDeathmatch)
				{
					g_reactions.start_activity.process(plyr);
					return true;
				}
				else if (activity == eActivityType::DefendSpecialCargo || activity == eActivityType::GunrunningDefend || activity == eActivityType::BikerDefend || args[3] == 238)
				{
					g_reactions.trigger_business_raid.process(plyr);
					return true;
				}
			}
			else if (activity == eActivityType::Tennis)
			{
				g_reactions.scripted_event_crash.process(plyr);
				return true;
			}

			if (g_protections.script_events.start_activity && !is_player_our_bodyguard(player->m_player_id))
			{
				g_reactions.start_activity.process(plyr);
				return true;
			}

			break;
		}
		case eRemoteEvent::InteriorControl:
		{
			int interior = (int)args[3];
			if (interior < 0 || interior > 171) // the upper bound will change after an update
			{
				g_reactions.null_function_kick.process(plyr);
				return true;
			}

			if (NETWORK::NETWORK_IS_ACTIVITY_SESSION() || !g_local_player || plyr->is_friend() || is_player_same_team(plyr->id()) || player_is_driver(plyr))
				break;

			g_reactions.send_to_interior.process(plyr);
			auto cped = plyr->get_ped();
			auto dist = cped ? math::distance_between_vectors(*cped->get_position(), *g_local_player->get_position()) : -1;
			g_log.log_additional(std::format("InteriorControl: {}, {}", plyr->m_name, dist));

			break;
		}
		case eRemoteEvent::DestroyPersonalVehicle: g_reactions.destroy_personal_vehicle.process(plyr); return true;
		case eRemoteEvent::KickFromInterior:
			if (scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[self::id].SimpleInteriorData.Owner != player->m_player_id)
			{
				g_reactions.kick_from_interior.process(plyr);
				return true;
			}
			break;
		case eRemoteEvent::TriggerCEORaid:
		{
			if (auto script = gta_util::find_script_thread("freemode"_J))
				if (script->m_net_component && ((CGameScriptHandlerNetComponent*)script->m_net_component)->m_host
				    && ((CGameScriptHandlerNetComponent*)script->m_net_component)->m_host->m_net_game_player != player)
					g_reactions.trigger_business_raid.process(plyr);

			LOG(WARNING) << "CEO RAID WAS BLOCKED";
			return true;
		}
		case eRemoteEvent::StartScriptProceed:
		{
			// TODO: Breaks stuff
			if (auto script = gta_util::find_script_thread("freemode"_J))
			{
				auto comp = (CGameScriptHandlerNetComponent*)script->m_net_component;
				if (comp && comp->m_host && comp->m_host->m_net_game_player != player)
				{
					g_reactions.start_script_proceed.process(plyr);
					return true;
				}
			}
			break;
		}
		case eRemoteEvent::StartScriptBegin:
		{
			int64_t script_id = args[3];

			if (!protection::should_allow_script_launch(script_id))
			{
				g_reactions.start_script_begin.process(plyr);
				g_log.log_additional(std::format("Blocked StartScriptBegin: {}, {}", script_id, plyr->m_name));
				return true;
			}

			LOGF(VERBOSE, "Allowed StartScriptBegin: {}, {}", script_id, plyr->m_name);
			break;
		}
		}

		// detect pasted menus setting args[1] to something other than PLAYER_ID()
		if (*(int*)&args[1] != player->m_player_id && player->m_player_id != -1)
		{
			g_reactions.tse_sender_mismatch.process(plyr);
			LOGF(WARNING, "tse_sender_mismatch Player= {}, Hash= {}, Sender= {}", player->get_name(), (int)args[0], args[1]);
			return true;
		}

		if (g_debug.log_script_events)
		{
			std::string script_args = "{ ";
			for (int i = 0; i < args_count; i++)
			{
				if (i)
					script_args += ", ";

				script_args += std::to_string((int)args[i]);
			}
			script_args += " };";

			g_log.log_additional(std::format("Script Event: Player: {} | Args: {}", player->get_name(), script_args));
		}

		return false;
	}
}
