#include "core/data/misc.hpp"
#include "core/data/protections.hpp"
#include "core/data/reactions.hpp"
#include "core/data/session.hpp"
#include "core/data/syncing_player.hpp"
#include "fiber_pool.hpp"
#include "gta/enums.hpp"
#include "gta/joaat.hpp"
#include "gta/net_game_event.hpp"
#include "gta/weapon_info_manager.hpp"
#include "gta_util.hpp"
#include "hooking/hooking.hpp"
#include "logger/logger.hpp"
#include "services/players/player_service.hpp"
#include "util/math.hpp"
#include "util/player.hpp"

#include <base/CObject.hpp>
#include <network/CNetGamePlayer.hpp>
#include <script/globals/GlobalPlayerBD.hpp>

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

	static inline bool is_valid_weapon(rage::joaat_t hash)
	{
		for (const auto& info : g_pointers->m_gta.m_weapon_info_manager->m_item_infos)
			if (info && info->m_name == hash && info->GetClassId() == "cweaponinfo"_J)
				return true;

		return false;
	}

	void scan_explosion_event(CNetGamePlayer* player, rage::datBitBuffer* buffer, player_ptr plyr)
	{
		uint16_t f186;
		uint16_t targetEntity;
		uint16_t ownerNetId;
		uint16_t f214;
		eExplosionTag explosionType;
		float damageScale;

		float posX;
		float posY;
		float posZ;

		bool f242;
		uint16_t f104;
		float cameraShake;

		bool isAudible;
		bool f189;
		bool isInvisible;
		bool f126;
		bool addOwnedExplosion;

		bool hasTargetEnt2;
		uint16_t targetEnt2;

		float unkX;
		float unkY;
		float unkZ;

		bool f190;
		bool f191;

		uint32_t f164;

		float posX224;
		float posY224;
		float posZ224;

		bool f240;
		uint16_t f218;
		bool f216;

		// clang-format off

		f186 = buffer->Read<uint16_t>(16);
		targetEntity = buffer->Read<uint16_t>(13);
		ownerNetId  = buffer->Read<uint16_t>(13);
		f214        = buffer->Read<uint16_t>(13);               // 1604+
		explosionType = (eExplosionTag)buffer->ReadSigned<int>(8);// 1604+ bit size
		damageScale = buffer->Read<int>(8) / 255.0f;

		posX        = buffer->ReadSignedFloat(22, 27648.0f);
		posY        = buffer->ReadSignedFloat(22, 27648.0f);
		posZ        = buffer->ReadFloat(22, 4416.0f) - 1700.0f;

		f242        = buffer->Read<bool>(1);
		f104        = buffer->Read<uint16_t>(16);
		cameraShake = buffer->Read<int>(8) / 127.0f;

		isAudible   = buffer->Read<uint8_t>(1);
		f189        = buffer->Read<uint8_t>(1);
		isInvisible = buffer->Read<uint8_t>(1);
		f126        = buffer->Read<uint8_t>(1);
		addOwnedExplosion = buffer->Read<uint8_t>(1);
		buffer->Read<uint8_t>(1);

		hasTargetEnt2 = buffer->Read<bool>(1);// 1604+
		targetEnt2 = buffer->Read<uint16_t>(13);

		unkX        = buffer->ReadSignedFloat(16, 1.1f);
		unkY        = buffer->ReadSignedFloat(16, 1.1f);
		unkZ        = buffer->ReadSignedFloat(16, 1.1f);

		f190     	= buffer->Read<uint8_t>(1);
		f191        = buffer->Read<uint8_t>(1);
		f164        = buffer->Read<uint32_t>(32);

		if (f242)
		{
			posX224 = buffer->ReadSignedFloat(31, 27648.0f);
			posY224 = buffer->ReadSignedFloat(31, 27648.0f);
			posZ224 = buffer->ReadFloat(31, 4416.0f) - 1700.0f;
		}
		else
		{
			posX224 = 0;
			posY224 = 0;
			posZ224 = 0;
		}

		auto f168 = buffer->Read<uint32_t>(32);// >= 1868: f_168


		f240 = buffer->Read<uint8_t>(1);
		if (f240)
		{
			f218 = buffer->Read<uint16_t>(16);

			if (f191)
			{
				f216 = buffer->Read<uint8_t>(8);
			}
		}

		buffer->Seek(0);

		auto object = g_pointers->m_gta.m_get_net_object(*g_pointers->m_gta.m_network_object_mgr, ownerNetId, true);
		auto entity = object ? object->GetGameObject() : nullptr;

		if (addOwnedExplosion && entity && entity->m_entity_type == 4 && reinterpret_cast<CPed*>(entity)->m_player_info
			&& player->m_player_info->m_ped && player->m_player_info->m_ped->m_net_object
			&& ownerNetId != player->m_player_info->m_ped->m_net_object->m_object_id)
			// {} sent an EXPLOSION_EVENT with addOwnedExplosion enabled & wrong owner
			g_reactions.blamed_explosion_detected.process(plyr, nullptr);

		if (g_misc.log_explosion_event)
			LOGF(WARNING,
			    "Explosion Event: {} (Dist- {})",
			    plyr->m_name,
			    math::distance_between_vectors(*plyr->get_ped()->get_position(), {posX, posY, posZ}));

		if(explosionType == EXP_TAG_SUBMARINE_BIG && damageScale > 9998.0f && isAudible == true && isInvisible == false && cameraShake > 9998.0f)
			LOGF(WARNING,
			    "Yim. Blame Exp.: {} (Dist- {})",
			    plyr->m_name,
			    math::distance_between_vectors(*plyr->get_ped()->get_position(), {posX, posY, posZ}));

		// clang-format on
	}

	bool scan_play_sound_event(player_ptr plyr, rage::datBitBuffer& buffer)
	{
		bool is_entity = buffer.Read<bool>(1);
		std::int16_t entity_net_id;
		rage::fvector3 position;

		if (is_entity)
		{
			entity_net_id = buffer.Read<uint16_t>(13);
		}
		else
		{
			position.x = buffer.ReadSignedFloat(19, 1337.0f);
			position.y = buffer.ReadSignedFloat(19, 1337.0f);
			position.z = buffer.ReadFloat(19, 1337.0f);
		}

		bool has_ref        = buffer.Read<bool>(1);
		uint32_t ref_hash   = has_ref ? buffer.Read<uint32_t>(32) : 0;
		uint32_t sound_hash = buffer.Read<uint32_t>(32);
		uint8_t sound_id    = buffer.Read<uint8_t>(8);

		bool has_script_hash = buffer.Read<bool>(1);
		uint32_t script_hash = has_script_hash ? buffer.Read<uint32_t>(32) : 0;

		static const std::unordered_set<uint32_t> blocked_ref_hashes = {"Arena_Vehicle_Mod_Shop_Sounds"_J, "CELEBRATION_SOUNDSET"_J, "DLC_AW_Arena_Office_Planning_Wall_Sounds"_J, "DLC_AW_Arena_Spin_Wheel_Game_Frontend_Sounds"_J, "DLC_Biker_SYG_Sounds"_J, "DLC_BTL_SECURITY_VANS_RADAR_PING_SOUNDS"_J, "DLC_BTL_Target_Pursuit_Sounds"_J, "DLC_GR_Bunker_Door_Sounds"_J, "DLC_GR_CS2_Sounds"_J, "DLC_IO_Warehouse_Mod_Garage_Sounds"_J, "DLC_MPSUM2_HSW_Up_Sounds"_J, "DLC_sum20_Business_Battle_AC_Sounds"_J, "DLC_TG_Running_Back_Sounds"_J, "dlc_vw_table_games_frontend_sounds"_J, "dlc_xm_facility_entry_exit_sounds"_J, "Frontend"_J, "GTAO_Boss_Goons_FM_Soundset"_J, "GTAO_Exec_SecuroServ_Computer_Sounds"_J, "GTAO_Exec_SecuroServ_Warehouse_PC_Sounds"_J, "GTAO_Script_Doors_Faded_Screen_Sounds"_J, "GTAO_SMG_Hangar_Computer_Sounds"_J, "HUD_AMMO_SHOP_SOUNDSET"_J, "HUD_FRONTEND_CUSTOM_SOUNDSET"_J, "HUD_FRONTEND_DEFAULT_SOUNDSET"_J, "HUD_FRONTEND_MP_SOUNDSET"_J, "HUD_FRONTEND_MP_COLLECTABLE_SOUNDS"_J, "HUD_FRONTEND_TATTOO_SHOP_SOUNDSET"_J, "HUD_FRONTEND_CLOTHESSHOP_SOUNDSET"_J, "HUD_FRONTEND_STANDARD_PICKUPS_NPC_SOUNDSET"_J, "HUD_FRONTEND_VEHICLE_PICKUPS_NPC_SOUNDSET"_J, "HUD_FRONTEND_WEAPONS_PICKUPS_NPC_SOUNDSET"_J, "HUD_FREEMODE_SOUNDSET"_J, "HUD_MINI_GAME_SOUNDSET"_J, "HUD_AWARDS"_J, "JA16_Super_Mod_Garage_Sounds"_J, "Low2_Super_Mod_Garage_Sounds"_J, "MissionFailedSounds"_J, "MP_CCTV_SOUNDSET"_J, "MP_LOBBY_SOUNDS"_J, "MP_MISSION_COUNTDOWN_SOUNDSET"_J, "Phone_SoundSet_Default"_J, "Phone_SoundSet_Glasses_Cam"_J, "Phone_SoundSet_Prologue"_J, "Phone_SoundSet_Franklin"_J, "Phone_SoundSet_Michael"_J, "Phone_SoundSet_Trevor"_J, "PLAYER_SWITCH_CUSTOM_SOUNDSET"_J, "RESPAWN_ONLINE_SOUNDSET"_J, "TATTOOIST_SOUNDS"_J, "WastedSounds"_J, "WEB_NAVIGATION_SOUNDS_PHONE"_J};
		static const std::unordered_set<uint32_t> blocked_sound_hashes = {"Remote_Ring"_J, "COP_HELI_CAM_ZOOM"_J, "Object_Dropped_Remote"_J};
		static const std::unordered_set<uint32_t> blocked_script_hashes = {"main_persistent"_J, "shop_controller"_J};

		auto should_block = [&] {
			if (blocked_ref_hashes.contains(ref_hash) || blocked_sound_hashes.contains(sound_hash)
			    || blocked_script_hashes.contains(script_hash))
				return true;

			switch (sound_hash)
			{
			case "DLC_XM_Explosions_Orbital_Cannon"_J:
			{
				if (is_entity)
					return true;

				if (!scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[plyr->id()].OrbitalBitset.IsSet(eOrbitalBitset::kOrbitalCannonActive))
					return true;

				static const std::unordered_set<uint32_t> valid_script_hashes = {"am_mp_defunct_base"_J, "am_mp_orbital_cannon"_J, "fm_mission_controller_2020"_J, "fm_mission_controller"_J};
				if (!valid_script_hashes.contains(script_hash))
					return true;

				break;
			}
			}

			switch (ref_hash)
			{
			case "GTAO_Biker_Modes_Soundset"_J:
			case "DLC_Biker_Sell_Postman_Sounds"_J:
			{
				if (is_entity)
					return true;

				if (script_hash != "gb_biker_contraband_sell"_J)
					return true;

				break;
			}
			case "DLC_AW_General_Sounds"_J:
			{
				if (sound_hash != "Airhorn_Blast_Long"_J)
					return true;

				if (script_hash != "gb_casino_heist"_J)
					return true;

				if (!gta_util::find_script_thread("gb_casino_heist"_J))
					return true;

				break;
			}
			case "GTAO_FM_Events_Soundset"_J:
			{
				if (!is_entity)
					return true;

				if (sound_hash != "Explosion_Countdown"_J)
					return true;

				break;
			}
			}

			return false;
		}();

		if (should_block)
			g_log.log_additional(std::format("NETWORK_PLAY_SOUND_EVENT from {} with is_entity: {}, ref_hash: {:X}, sound_hash: {:X}, sound_id: {}, script_hash: {:X}", plyr->m_name, is_entity ? "T" : "F", ref_hash, sound_hash, sound_id, script_hash));

		buffer.Seek(0);
		return should_block;
	}

	void hooks::received_event(rage::netEventMgr* event_manager, CNetGamePlayer* source_player, CNetGamePlayer* target_player, uint16_t event_id, int event_index, int event_handled_bitset, int buffer_size, rage::datBitBuffer* buffer)
	{
		auto send_ack_event = [=]() {
			g_pointers->m_gta.m_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
		};

		if (event_id > 91u)
			return send_ack_event();

		const auto event_name = *(char**)((DWORD64)event_manager + 8i64 * event_id + 243376);
		if (event_name == nullptr || source_player == nullptr || source_player->m_player_id < 0 || source_player->m_player_id >= 32)
			return send_ack_event();

		auto plyr     = g_player_service->get_by_id(source_player->m_player_id);
		auto tar_plyr = g_player_service->get_by_id(target_player->m_player_id);

		if (!plyr)
		{
			LOG(WARNING) << "received_event player not found: " << source_player->get_name();
			return send_ack_event();
		}

		if (plyr->block_net_events)
			return send_ack_event();

		switch (static_cast<eNetworkEvents>(event_id))
		{
		case eNetworkEvents::KICK_VOTES_EVENT:
		{
			uint32_t player_bitfield = buffer->Read<uint32_t>(32);
			if (player_bitfield & (1 << target_player->m_player_id))
			{
				g_reactions.kick_vote.process(plyr);
				++g_session.num_vote_kicks;
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
					g_reactions.crash31.process(plyr, tar_plyr);
					return send_ack_event();
				}
			}
			else if (type == ScriptEntityChangeType::SetVehicleLockState)
			{
				if (g_local_player && g_local_player->m_vehicle && g_local_player->m_vehicle->m_net_object
				    && g_local_player->m_vehicle->m_net_object->m_object_id == entity)
				{
					LOGF(WARNING, "Blocked SetVehicleLockState from {} on our local vehicle", plyr->m_name);
					return send_ack_event();
				}
			}
			else if (type == ScriptEntityChangeType::SetVehicleExclusiveDriver)
			{
				if (g_local_player && g_local_player->m_vehicle && g_local_player->m_vehicle->m_net_object
				    && g_local_player->m_vehicle->m_net_object->m_object_id == entity)
				{
					LOGF(WARNING, "Blocked SetVehicleExclusiveDriver from {} on our local vehicle", plyr->m_name);
					g_reactions.vehicle_kick.process(plyr);
					if (g_block_reaction)
						return send_ack_event();
				}
			}
			else if (type == ScriptEntityChangeType::SetPedFacialIdleAnimOverride)
			{
				if (g_local_player && g_local_player->m_net_object && g_local_player->m_net_object->m_object_id)
				{
					LOGF(WARNING, "Blocked SetPedFacialIdleAnimOverride from {} on our local player", plyr->m_name);
					return send_ack_event();
				}
			}
			else if (type > ScriptEntityChangeType::SetVehicleExclusiveDriver || type < ScriptEntityChangeType::BlockingOfNonTemporaryEvents)
			{
				g_reactions.crash32.process(plyr, tar_plyr);
				return send_ack_event();
			}
			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::SCRIPTED_GAME_EVENT:
		{
			const auto scripted_game_event = std::make_unique<CScriptedGameEvent>();

			buffer->ReadDword(&scripted_game_event->m_args_size, 32);

			if (scripted_game_event->m_args_size > sizeof(scripted_game_event->m_args))
				return send_ack_event();

			buffer->ReadArray(&scripted_game_event->m_args, 8 * scripted_game_event->m_args_size);

			if (hooks::scripted_game_event(scripted_game_event.get(), source_player))
				return send_ack_event();

			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::NETWORK_CLEAR_PED_TASKS_EVENT:
		{
			int net_id = buffer->Read<int>(13);

			if (!plyr->is_friend() && g_protections.clear_ped_tasks && g_local_player && g_local_player->m_net_object
			    && g_local_player->m_net_object->m_object_id == net_id)
			{
				g_reactions.clear_ped_tasks.process(plyr);
				if (g_block_reaction)
					return send_ack_event();
			}

			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::RAGDOLL_REQUEST_EVENT:
		{
			int net_id = buffer->Read<int>(13);

			if (g_local_player && g_local_player->m_net_object && g_local_player->m_net_object->m_object_id == net_id)
			{
				if (!PED::IS_PED_RUNNING_RAGDOLL_TASK(self::ped))
					g_reactions.remote_ragdoll.process(plyr);
				if (g_block_reaction)
					return send_ack_event();
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
				g_reactions.report_cash_spawn.process(plyr);
			}

			break;
		}
		// player sending this event is a modder
		case eNetworkEvents::REPORT_MYSELF_EVENT:
		{
			auto p1 = buffer->Read<int>(32);
			auto p2 = buffer->Read<int>(32);
			g_log.log_additional(std::format("Received REPORT_MYSELF_EVENT from {} with parameters ({}, {})", plyr->m_name, p1, p2));
			g_reactions.anti_cheat_modder_detection.process(plyr);
			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::REQUEST_CONTROL_EVENT:
		{
			auto net_id = buffer->Read<int>(13);

			if (!NETWORK::NETWORK_IS_ACTIVITY_SESSION() // If we're in Freemode
			    && !plyr->is_friend() && g_protections.request_control_event && g_local_player->m_vehicle
			    && g_local_player->m_vehicle->m_net_object && g_local_player->m_vehicle->m_net_object->m_object_id == net_id //The request is for a vehicle we are currently in.
			)
			{
				if (NETWORK::NETWORK_IS_ACTIVITY_SESSION())
					g_log.log_additional(std::format("REQUEST_CONTROL_EVENT 0: {}", plyr->m_name));
				else if (is_player_same_team(plyr->id()))
					g_log.log_additional(std::format("REQUEST_CONTROL_EVENT 1: {}", plyr->m_name));
				else
				{
					auto val = player_is_driver(plyr, true);
					if (val == 1)
					{
						g_reactions.request_control_event_prob.process(plyr);
						if (g_block_reaction)
							return send_ack_event();
					}
					else if (val == 0)
					{
						g_reactions.request_control_event.process(plyr);
						if (g_block_reaction)
							return send_ack_event();
					}
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
				buffer->Read<int>(9);        // network rope id
				buffer->ReadSigned<int>(19); // pos x
				buffer->ReadSigned<int>(19); // pos y
				buffer->Read<int>(19);       // pos z
				buffer->ReadSigned<int>(19); // rot x
				buffer->ReadSigned<int>(19); // rot y
				buffer->Read<int>(19);       // rot z
				float max_length     = buffer->ReadSignedFloat(16, 100.0f);
				int type             = buffer->Read<int>(4);
				float initial_length = buffer->ReadSignedFloat(16, 100.0f);
				float min_length     = buffer->ReadSignedFloat(16, 100.0f);

				if (type == 0 || initial_length < min_length || max_length < min_length || max_length < 0.0f)
				{
					g_log.log_additional(std::format("{} sent a Rope that would crash the game. Script Hash: {:X}, Type: {}, Initial Length: {}, Min Length: {}, Max Length: {}", plyr->m_name, id.m_hash, type, initial_length, min_length, max_length));
					g_reactions.crash33.process(plyr);
					return send_ack_event();
				}
			}
			else if (type == WorldStateDataType::PopGroupOverride)
			{
				int pop_schedule = buffer->ReadSigned<int>(8); // Pop Schedule
				int pop_group    = buffer->Read<int>(32);      // Pop Group
				int percentage   = buffer->Read<int>(7);       // Percentage

				if (pop_group == 0 && (percentage == 0 || percentage == 103))
				{
					g_reactions.crash34.process(plyr, tar_plyr);
					return send_ack_event();
				}
			}
			else if (type > WorldStateDataType::VehiclePlayerLocking || type < WorldStateDataType::CarGen)
			{
				g_reactions.crash35.process(plyr, tar_plyr);
				return send_ack_event();
			}
			else if (type == WorldStateDataType::PopMultiplierArea && g_protections.stop_traffic && !NETWORK::NETWORK_IS_ACTIVITY_SESSION())
			{
				g_reactions.pop_multiplier_area.process(plyr, tar_plyr);
				return send_ack_event();
			}

			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::REMOVE_WEAPON_EVENT:
		{
			std::int16_t net_id = buffer->Read<std::int16_t>(13);
			Hash hash           = buffer->Read<Hash>(32);

			if (hash == "WEAPON_UNARMED"_J)
			{
				g_reactions.crash36.process(plyr, tar_plyr);
				return send_ack_event();
			}

			if (g_local_player && g_local_player->m_net_object && g_local_player->m_net_object->m_object_id == net_id)
			{
				g_reactions.remove_weapon.process(g_player_service->get_by_id(source_player->m_player_id));
				return send_ack_event();
			}

			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::GIVE_WEAPON_EVENT:
		{
			std::int16_t net_id = buffer->Read<std::int16_t>(13);

			if (g_local_player && g_local_player->m_net_object && g_local_player->m_net_object->m_object_id == net_id)
			{
				g_reactions.give_weapon.process(g_player_service->get_by_id(source_player->m_player_id));
				return send_ack_event();
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
					g_reactions.crash37.process(plyr, tar_plyr);
					return send_ack_event();
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
			if (g_misc.log_sound_event)
				LOG(WARNING) << "Sound Event (NETWORK_PLAY_SOUND_EVENT) from: " << plyr->m_name;

			if (plyr->m_play_sound_rate_limit.process())
			{
				g_reactions.nw_sound_spam_rate_limit.process(plyr, tar_plyr);
				return send_ack_event();
			}

			if (scan_play_sound_event(plyr, *buffer))
			{
				g_reactions.nw_sound_spam_scanned.process(plyr);
				// return send_ack_event(); // lets hear the sound
			}

			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::EXPLOSION_EVENT:
		{
			if (plyr->block_explosions)
				return send_ack_event();

			scan_explosion_event(source_player, buffer, plyr);
			break;
		}
		case eNetworkEvents::WEAPON_DAMAGE_EVENT:
		{
			auto damageType = buffer->Read<uint8_t>(2);
			auto weaponType = buffer->Read<uint32_t>(32); // weaponHash

			if (!is_valid_weapon(weaponType))
			{
				g_reactions.invalid_weapon_type.process(plyr);
				return send_ack_event();
			}

			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::NETWORK_PTFX_EVENT:
		{
			if (g_misc.log_ptfx_event)
				LOGF(WARNING,
				    "PTFX Event: {} (Dist- {})",
				    plyr->m_name,
				    math::distance_between_vectors(*plyr->get_ped()->get_position(), *g_local_player->get_position()));

			if (plyr->block_ptfx)
				return send_ack_event();

			if (plyr->whitelist_ptfx)
				break;

			if (plyr->m_ptfx_ratelimit.process())
			{
				if (plyr->m_ptfx_ratelimit.exceeded_last_process())
					g_reactions.ptfx_spam.process(plyr);

				return send_ack_event();
			}

			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::ACTIVATE_VEHICLE_SPECIAL_ABILITY_EVENT:
		{
			int16_t net_id = buffer->Read<int16_t>(13);

			if (is_local_vehicle_net_id(net_id))
			{
				g_reactions.veh_spec_ability_event.process(plyr, tar_plyr);
				if (g_block_reaction)
					return send_ack_event();
			}

			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::DOOR_BREAK_EVENT:
		{
			int16_t net_id = buffer->Read<int16_t>(13);

			if (is_local_vehicle_net_id(net_id))
			{
				g_reactions.break_door_event.process(plyr, tar_plyr);
				if (g_block_reaction)
					return send_ack_event();
			}

			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::CHANGE_RADIO_STATION_EVENT:
		{
			int16_t net_id = buffer->Read<int16_t>(13);

			if (!is_in_vehicle(plyr->get_ped(), g_local_player->m_vehicle))
			{
				g_reactions.change_radio_station.process(plyr, tar_plyr);
				if (g_block_reaction)
					return send_ack_event();
			}

			if (plyr->m_radio_station_change_rate_limit.process())
			{
				g_reactions.change_radio_station.process(plyr, tar_plyr);
				if (g_block_reaction)
					return send_ack_event();
			}

			buffer->Seek(0);
			break;
		}
		default: break;
		}

		return g_hooking->get_original<received_event>()(event_manager, source_player, target_player, event_id, event_index, event_handled_bitset, buffer_size, buffer);
	}
}
