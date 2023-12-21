#include "core/data/reactions.hpp"
#include "core/data/syncing_player.hpp"
#include "fiber_pool.hpp"
#include "gta/enums.hpp"
#include "gta/net_game_event.hpp"
#include "hooking.hpp"
#include "script/scriptIdBase.hpp"
#include "util/math.hpp"
#include "util/mobile.hpp"
#include "util/notify.hpp"

#include <base/CObject.hpp>
#include <network/CNetGamePlayer.hpp>

namespace big
{
	static void script_id_deserialize(CGameScriptId& id, rage::datBitBuffer& buffer)
	{
		id.m_hash      = buffer.Read<uint32_t>(32);
		id.m_timestamp = buffer.Read<uint32_t>(32);

		if (buffer.Read<bool>(1))
			id.m_position_hash = buffer.Read<uint32_t>(32);

		if (buffer.Read<bool>(1))
			id.m_instance_id = buffer.Read<int32_t>(8);
	}

	void hooks::received_event(rage::netEventMgr* event_manager, CNetGamePlayer* source_player, CNetGamePlayer* target_player, uint16_t event_id, int event_index, int event_handled_bitset, int buffer_size, rage::datBitBuffer* buffer)
	{
		if (event_id > 91u)
		{
			g_pointers->m_gta.m_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
			return;
		}

		const auto event_name = *(char**)((DWORD64)event_manager + 8i64 * event_id + 243376);
		if (event_name == nullptr || source_player == nullptr || source_player->m_player_id < 0 || source_player->m_player_id >= 32)
		{
			g_pointers->m_gta.m_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
			return;
		}

		auto plyr = g_player_service->get_by_id(source_player->m_player_id);

		if (plyr && plyr->block_net_events)
		{
			g_pointers->m_gta.m_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
			return;
		}

		switch (static_cast<eNetworkEvents>(event_id))
		{
		case eNetworkEvents::KICK_VOTES_EVENT:
		{
			uint32_t player_bitfield = buffer->Read<uint32_t>(32);
			if (player_bitfield & (1 << target_player->m_player_id))
				g_reactions.kick_vote.process(plyr, false, Infraction::VOTE_KICK, true, true);
			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::NETWORK_INCREMENT_STAT_EVENT:
		{
			const auto increment_stat_event = std::make_unique<CNetworkIncrementStatEvent>();
			buffer->ReadDword(&increment_stat_event->m_stat, 0x20);
			buffer->ReadDword(&increment_stat_event->m_amount, 0x20);
			if (hooks::increment_stat_event(increment_stat_event.get(), source_player))
			{
				g_pointers->m_gta.m_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
				return;
			}
			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::SCRIPT_ENTITY_STATE_CHANGE_EVENT:
		{
			uint16_t entity = buffer->Read<uint16_t>(13);
			auto type       = buffer->Read<ScriptEntityChangeType>(4);
			uint32_t unk    = buffer->Read<uint32_t>(32);
			if (type == ScriptEntityChangeType::SettingOfTaskVehicleTempAction)
			{
				uint16_t ped_id = buffer->Read<uint16_t>(13);
				uint32_t action = buffer->Read<uint32_t>(8);

				if ((action >= 15 && action <= 18) || action == 33)
				{
					g_pointers->m_gta.m_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
					notify::crash_blocked(plyr, 31);
					return;
				}
			}
			else if (type > ScriptEntityChangeType::SetVehicleExclusiveDriver || type < ScriptEntityChangeType::BlockingOfNonTemporaryEvents)
			{
				notify::crash_blocked(plyr, 32);
				g_pointers->m_gta.m_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
				return;
			}
			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::SCRIPTED_GAME_EVENT:
		{
			const auto scripted_game_event = std::make_unique<CScriptedGameEvent>();
			buffer->ReadDword(&scripted_game_event->m_args_size, 32);
			if (scripted_game_event->m_args_size - 1 <= 0x1AF)
				buffer->ReadArray(&scripted_game_event->m_args, 8 * scripted_game_event->m_args_size);

			if (hooks::scripted_game_event(scripted_game_event.get(), source_player))
			{
				g_pointers->m_gta.m_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);

				return;
			}
			buffer->Seek(0);

			break;
		}
		case eNetworkEvents::NETWORK_CLEAR_PED_TASKS_EVENT:
		{
			int net_id = buffer->Read<int>(13);

			if (g_local_player && g_local_player->m_net_object && g_local_player->m_net_object->m_object_id == net_id)
			{
				g_pointers->m_gta.m_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
				g_reactions.clear_ped_tasks.process(plyr, false, Infraction::NONE, false);
				return;
			}

			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::RAGDOLL_REQUEST_EVENT:
		{
			int net_id = buffer->Read<int>(13);

			if (g_local_player && g_local_player->m_net_object && g_local_player->m_net_object->m_object_id == net_id)
			{
				g_pointers->m_gta.m_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
				g_reactions.remote_ragdoll.process(plyr, false, Infraction::NONE, false);
				return;
			}

			buffer->Seek(0);
			break;
		}
		// Don't block this event, we still want to report this player
		// because if we still report others, our account seems less fishy
		case eNetworkEvents::REPORT_CASH_SPAWN_EVENT:
		{
			uint32_t money;

			buffer->Seek(64);
			buffer->ReadDword(&money, 32);
			buffer->Seek(0);

			if (money >= 2000)
			{
				g_reactions.report_cash_spawn.process(plyr, false, Infraction::CASH_SPAWN, true);
			}

			break;
		}
		// player sending this event is a modder
		case eNetworkEvents::REPORT_MYSELF_EVENT:
		{
			g_reactions.game_anti_cheat_modder_detection.process(plyr, false, Infraction::TRIGGERED_ANTICHEAT, true);
			break;
		}
		case eNetworkEvents::REQUEST_CONTROL_EVENT:
		{
			auto net_id = buffer->Read<int>(13);
			if (g_local_player && g_local_player->m_vehicle && g_local_player->m_vehicle->m_net_object
			    && g_local_player->m_vehicle->m_net_object->m_object_id == net_id) //The request is for a vehicle we are currently in.
			{
				Vehicle personal_vehicle = mobile::mechanic::get_personal_vehicle();
				Vehicle veh              = g_pointers->m_gta.m_ptr_to_handle(g_local_player->m_vehicle);
				if (!NETWORK::NETWORK_IS_ACTIVITY_SESSION()     //If we're in Freemode.
				    || personal_vehicle == veh                  //Or we're in our personal vehicle.
				    || self::spawned_vehicles.contains(net_id)) // Or it's a vehicle we spawned.
				{
					g_pointers->m_gta.m_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset); // Tell them to get bent.
					g_reactions.request_control_event.process(plyr, false, Infraction::NONE, false);
					return;
				}
			}
			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::SCRIPT_WORLD_STATE_EVENT:
		{
			auto type = buffer->Read<WorldStateDataType>(4);
			buffer->Read<bool>(1);
			CGameScriptId id;
			script_id_deserialize(id, *buffer);

			if (type == WorldStateDataType::Rope)
			{
				buffer->Read<int>(9);    // network rope id
				buffer->Read<float>(19); // pos x
				buffer->Read<float>(19); // pos y
				buffer->Read<float>(19); // pos z
				buffer->Read<float>(19); // rot x
				buffer->Read<float>(19); // rot y
				buffer->Read<float>(19); // rot z
				buffer->Read<float>(16); // length
				int type             = buffer->Read<int>(4);
				float initial_length = buffer->Read<float>(16);
				float min_length     = buffer->Read<float>(16);

				if (type == 0 || initial_length < min_length) // https://docs.fivem.net/natives/?_0xE832D760399EB220
				{
					notify::crash_blocked(plyr, 33);
					g_pointers->m_gta.m_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
					return;
				}
			}
			else if (type == WorldStateDataType::PopGroupOverride)
			{
				int pop_schedule = buffer->ReadSigned<int>(8); // Pop Schedule
				int pop_group    = buffer->Read<int>(32);      // Pop Group
				int percentage   = buffer->Read<int>(7);       // Percentage

				if (pop_group == 0 && (percentage == 0 || percentage == 103))
				{
					notify::crash_blocked(plyr, 34);
					g_pointers->m_gta.m_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
					return;
				}
			}
			else if (type > WorldStateDataType::VehiclePlayerLocking || type < WorldStateDataType::CarGen)
			{
				notify::crash_blocked(plyr, 35);
				g_pointers->m_gta.m_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
				return;
			}

			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::REMOVE_WEAPON_EVENT:
		{
			int net_id    = buffer->Read<int>(13);
			uint32_t hash = buffer->Read<uint32_t>(32);

			if (hash == RAGE_JOAAT("WEAPON_UNARMED"))
			{
				notify::crash_blocked(plyr, 36);
				g_pointers->m_gta.m_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
				return;
			}

			if (g_local_player && g_local_player->m_net_object && g_local_player->m_net_object->m_object_id == net_id)
			{
				g_reactions.remove_weapon.process(g_player_service->get_by_id(source_player->m_player_id), false, Infraction::REMOVE_WEAPON, true);
				g_pointers->m_gta.m_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
				return;
			}

			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::REMOVE_ALL_WEAPONS_EVENT:
		{
			int net_id = buffer->Read<int>(13);

			if (g_local_player && g_local_player->m_net_object && g_local_player->m_net_object->m_object_id == net_id)
			{
				g_reactions.remove_weapon.process(g_player_service->get_by_id(source_player->m_player_id), false, Infraction::REMOVE_WEAPON, true);
				g_pointers->m_gta.m_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
				return;
			}

			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::GIVE_WEAPON_EVENT:
		{
			int net_id = buffer->Read<int>(13);

			if (g_local_player && g_local_player->m_net_object && g_local_player->m_net_object->m_object_id == net_id)
			{
				auto freemode = !NETWORK::NETWORK_IS_ACTIVITY_SESSION();

				if (freemode)
				{
					g_reactions.give_weapon.process(g_player_service->get_by_id(source_player->m_player_id), false, Infraction::GIVE_WEAPON, true);
					g_pointers->m_gta.m_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
					return;
				}

				LOG(WARNING) << std::format("Received GIVE_WEAPON_EVENT from {} during activity", source_player->get_name());
			}
			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::GIVE_CONTROL_EVENT:
		{
			uint32_t timestamp                = buffer->Read<uint32_t>(32);
			int count                         = buffer->Read<int>(2);
			bool all_objects_migrate_together = buffer->Read<bool>(1);
			eNetObjType sync_type;

			if (count > 3)
			{
				count = 3;
			}

			for (int i = 0; i < count; i++)
			{
				int net_id              = buffer->Read<int>(13);
				eNetObjType object_type = buffer->Read<eNetObjType>(4);
				int migration_type      = buffer->Read<int>(3);

				if (object_type < eNetObjType::NET_OBJ_TYPE_AUTOMOBILE || object_type > eNetObjType::NET_OBJ_TYPE_TRAIN)
				{
					notify::crash_blocked(plyr, 37);
					g_pointers->m_gta.m_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
					return;
				}

				sync_type = object_type;
			}

			buffer->Seek(0);

			if (count)
			{
				m_syncing_player      = source_player;
				m_syncing_object_type = sync_type;
			}
			break;
		}
		case eNetworkEvents::NETWORK_PLAY_SOUND_EVENT:
		{
			if (plyr && plyr->m_play_sound_rate_limit.process())
			{
				if (plyr->m_play_sound_rate_limit.exceeded_last_process())
					g_reactions.sound_spam.process(plyr, false, Infraction::NONE, false);

				g_pointers->m_gta.m_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
				return;
			}

			bool is_entity = buffer->Read<bool>(1);
			std::int16_t entity_net_id;
			rage::fvector3 position;
			uint32_t ref_hash;

			if (is_entity)
				entity_net_id = buffer->Read<std::int16_t>(13);
			else
			{
				position.x = buffer->ReadSignedFloat(19, 1337.0f);
				position.y = buffer->ReadSignedFloat(19, 1337.0f);
				position.z = buffer->ReadFloat(19, 1337.0f);
			}

			bool has_ref = buffer->Read<bool>(1);
			if (has_ref)
				ref_hash = buffer->Read<uint32_t>(32);

			uint32_t sound_hash = buffer->Read<uint32_t>(32);

			if (sound_hash == RAGE_JOAAT("Remote_Ring") && plyr)
			{
				g_reactions.sound_spam.process(plyr, false, Infraction::NONE, false);
				return;
			}

			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::EXPLOSION_EVENT:
		{
			if (plyr && plyr->block_explosions)
			{
				g_pointers->m_gta.m_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
				return;
			}

			break;
		}
		default: break;
		}

		return g_hooking->get_original<received_event>()(event_manager, source_player, target_player, event_id, event_index, event_handled_bitset, buffer_size, buffer);
	}
}
