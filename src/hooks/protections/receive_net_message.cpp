#include "backend/command.hpp"
#include "backend/player_command.hpp"
#include "core/data/debug.hpp"
#include "core/data/kick_reasons.hpp"
#include "core/data/packet_types.hpp"
#include "core/data/reactions.hpp"
#include "core/data/session.hpp"
#include "core/data/unknown_player.hpp"
#include "gta/joaat.hpp"
#include "gta/net_game_event.hpp"
#include "gta/net_object_mgr.hpp"
#include "gta_util.hpp"
#include "hooking/hooking.hpp"
#include "natives.hpp"
#include "packet.hpp"
#include "script/scriptIdBase.hpp"
#include "services/blocked_players/blocked_players.hpp"
#include "services/custom_chat_buffer.hpp"
#include "services/notifications/notification_service.hpp"
#include "services/players/player_service.hpp"
#include "util/chat.hpp"

#include <network/P2pSecurity.hpp>

namespace big
{
	static player_ptr dummy_player = std::make_shared<player>(nullptr, 0);

	static bool try_read_secondary_header(rage::datBitBuffer& buffer)
	{
		auto data = buffer.Read<std::uint32_t>(20);
		return data == 0x8C253 || data == 0x8924F;
	}

	static bool get_msg_type(rage::eNetMessage& msgType, rage::datBitBuffer& buffer)
	{
		uint32_t pos;
		uint32_t magic;
		uint32_t length;
		uint32_t extended{};
		if ((buffer.m_flagBits & 2) != 0 || (buffer.m_flagBits & 1) == 0 ? (pos = buffer.m_curBit) : (pos = buffer.m_maxBit),
		    buffer.m_bitsRead + 15 > pos || !buffer.ReadDword(&magic, 14) || magic != 0x3246 || !buffer.ReadDword(&extended, 1))
		{
			msgType = rage::eNetMessage::MsgInvalid;
			return false;
		}
		length = extended ? 16 : 8;
		if ((buffer.m_flagBits & 1) == 0 ? (pos = buffer.m_curBit) : (pos = buffer.m_maxBit),
		    length + buffer.m_bitsRead <= pos && buffer.ReadDword((uint32_t*)&msgType, length))
			return true;
		else
			return false;
	}

	// static void script_id_deserialize(CGameScriptId& id, rage::datBitBuffer& buffer)
	// {
	// 	id.m_hash      = buffer.Read<uint32_t>(32);
	// 	id.m_timestamp = buffer.Read<uint32_t>(32);

	// 	if (buffer.Read<bool>(1))
	// 		id.m_position_hash = buffer.Read<uint32_t>(32);
	// 	else
	// 		id.m_position_hash = 0;

	// 	if (buffer.Read<bool>(1))
	// 		id.m_instance_id = buffer.Read<int32_t>(8);
	// 	else
	// 		id.m_instance_id = -1;
	// }

	// static void script_id_serialize(CGameScriptId& id, rage::datBitBuffer& buffer)
	// {
	// 	buffer.Write<uint32_t>(id.m_hash, 32);
	// 	buffer.Write<uint32_t>(id.m_timestamp, 32);

	// 	if (id.m_position_hash != 0)
	// 	{
	// 		buffer.Write<bool>(true, 1);
	// 		buffer.Write<uint32_t>(id.m_position_hash, 32);
	// 	}
	// 	else
	// 	{
	// 		buffer.Write<bool>(false, 1);
	// 	}

	// 	if (id.m_instance_id != -1)
	// 	{
	// 		buffer.Write<bool>(true, 1);
	// 		buffer.Write<int32_t>(id.m_instance_id, 8);
	// 	}
	// 	else
	// 	{
	// 		buffer.Write<bool>(false, 1);
	// 	}
	// }

	static void log_net_message(rage::eNetMessage message_type, rage::datBitBuffer& data_buffer, rage::netEvent* event, rage::SecurityPeer* sec_peer)
	{
		if (message_type != rage::eNetMessage::MsgCloneSync && message_type != rage::eNetMessage::MsgPackedCloneSyncACKs && message_type != rage::eNetMessage::MsgPackedEvents && message_type != rage::eNetMessage::MsgPackedReliables && message_type != rage::eNetMessage::MsgPackedEventReliablesMsgs && message_type != rage::eNetMessage::MsgNetArrayMgrUpdate && message_type != rage::eNetMessage::MsgNetArrayMgrSplitUpdateAck && message_type != rage::eNetMessage::MsgNetArrayMgrUpdateAck && message_type != rage::eNetMessage::MsgScriptHandshakeAck && message_type != rage::eNetMessage::MsgScriptHandshake && message_type != rage::eNetMessage::MsgScriptJoin && message_type != rage::eNetMessage::MsgScriptJoinAck && message_type != rage::eNetMessage::MsgScriptJoinHostAck && message_type != rage::eNetMessage::MsgRequestObjectIds && message_type != rage::eNetMessage::MsgInformObjectIds && message_type != rage::eNetMessage::MsgNetTimeSync)
		{
			const char* packet_type = "<UNKNOWN>";
			for (const auto& p : packet_types)
				if (p.second == (int)message_type)
				{
					packet_type = p.first;
					break;
				}

			std::string name = "";
			rage::rlGamerHandle rid{};

			if (sec_peer)
			{
				if (sec_peer->m_info.handle.m_platform == 3)
					rid = sec_peer->m_info.handle;
				else if (sec_peer->m_unverified_handle.m_platform == 3)
					rid = sec_peer->m_unverified_handle;
				name = sec_peer->m_info.name;
			}

			if (name.empty())
				name = "???";

			g_log.log_additional(std::format("PKT | {} (0x{:X}) [size=0x{:X}, cxnId={:X}, peerId={}, msgId={}] from {} ({})",
			    packet_type,
			    (int)message_type,
			    data_buffer.GetDataLength(),
			    event->m_connection_identifier,
			    event->m_peer_id,
			    event->m_msg_id,
			    name,
			    rid.m_rockstar_id));
		}
	}

	static bool is_host_of_session(rage::snSession* session, rage::joaat_t peer_id)
	{
		if (!session || peer_id == -1)
			return false;

		if (auto player = session->get_player_by_token(session->m_host_token))
			return player->m_player_data.m_peer_id_2 == peer_id;

		return false;
	}

	static const char* should_block_script_control_request(rage::joaat_t hash)
	{
		if (NETWORK::NETWORK_IS_ACTIVITY_SESSION() || NETWORK::NETWORK_IS_IN_TRANSITION() || NETWORK::NETWORK_IS_TRANSITION_BUSY())
			return nullptr;

		switch (hash)
		{
		case "freemode"_J:
		{
			if (g_session.force_freemode_host)
				return "freemode";
			break;
		}
		case "fmmc_launcher"_J:
		{
			if (g_session.force_fmmc_launcher_host)
				return "fmmc_launcher";
			break;
		}
		case "am_launcher"_J:
		{
			if (g_session.force_am_launcher_host)
				return "am_launcher";
			break;
		}
		}

		return nullptr;
	}

	bool hooks::receive_net_message(void* a1, rage::netConnectionManager* net_cxn_mgr, rage::netEvent* event)
	{
		void* message_data;
		int message_size;
		rage::netConnection::InFrame* frame  = nullptr;
		rage::netEventConnectionError* error = nullptr;

		if (event->get_event_type() == rage::netEvent::Type::ConnectionRequested || event->get_event_type() == rage::netEvent::Type::FrameReceived)
		{
			frame = reinterpret_cast<rage::netConnection::InFrame*>(event);
			if (frame->m_data == nullptr || frame->m_length == 0 || frame->m_connection_identifier == 2)
				return g_hooking->get_original<hooks::receive_net_message>()(a1, net_cxn_mgr, event);

			message_data = frame->m_data;
			message_size = frame->m_length;
		}
		else if (event->get_event_type() == rage::netEvent::Type::ConnectionError)
		{
			error = reinterpret_cast<rage::netEventConnectionError*>(event);
			if (error->m_size == 0)
				return g_hooking->get_original<hooks::receive_net_message>()(a1, net_cxn_mgr, event);

			message_data = error->m_data;
			message_size = error->m_size;
		}
		else
			return g_hooking->get_original<hooks::receive_net_message>()(a1, net_cxn_mgr, event);

		rage::datBitBuffer buffer(message_data, message_size);
		buffer.m_flagBits = 1;

		if (try_read_secondary_header(buffer))
		{
			buffer            = rage::datBitBuffer(((char*)message_data) + 7, message_size - 7);
			buffer.m_flagBits = 1;
		}
		else
			buffer.Seek(0);

		rage::eNetMessage msgType;

		if (!get_msg_type(msgType, buffer))
		{
			LOGF(WARNING, "Message cannot be parsed. cxn id {}", event->m_connection_identifier);
			return g_hooking->get_original<hooks::receive_net_message>()(a1, net_cxn_mgr, event);
		}

		rage::snSession* session = gta_util::get_network()->m_game_session_ptr; // game unless proven otherwise

		if (gta_util::get_network()->m_transition_session_ptr
		    && gta_util::get_network()->m_transition_session_ptr->m_connection_identifier == event->m_connection_identifier)
			session = gta_util::get_network()->m_transition_session_ptr;

		player_ptr _player = nullptr, unkown_player = dummy_player;
		rock_id rockstar_id;

		for (uint32_t i = 0; i < gta_util::get_network()->m_game_session_ptr->m_player_count; i++)
			if (auto sn_player = gta_util::get_network()->m_game_session_ptr->m_players[i];
			    sn_player && sn_player->m_player_data.m_peer_id_2 == event->m_peer_id)
			{
				rockstar_id = sn_player->m_player_data.m_gamer_handle.m_rockstar_id;
				_player     = g_player_service->get_by_host_token(sn_player->m_player_data.m_host_token);

				if (!_player)
				{
					auto p = g_unknown_players.find(rockstar_id);
					if (p == g_unknown_players.end())
					{
						auto plyr           = std::make_shared<player>(nullptr, sn_player->m_player_data.m_host_token);
						const auto [itr, _] = g_unknown_players.insert({rockstar_id, std::move(plyr)});
						unkown_player       = itr->second;
						auto str            = std::to_string(rockstar_id);
						strcpy(unkown_player->m_name, str.c_str());
					}
					else
						unkown_player = p->second;
				}

				break;
			}

		int sec_id = 0;
		if (frame)
			sec_id = frame->m_security_id;
		else if (auto cxn = g_pointers->m_gta.m_get_connection_peer(net_cxn_mgr, error->m_peer_id))
			sec_id = cxn->m_security_id;

		auto peer = g_pointers->m_gta.m_get_peer_by_security_id(sec_id); // shouldn't be null in most cases, contains unspoofable data

		auto plyr        = _player ? _player : unkown_player;
		auto player_name = _player ? _player->m_name : (peer ? peer->m_info.name : unkown_player->m_name);

		auto get_name_rid = [player_name, rockstar_id] {
			return std::format("{} ({})", player_name, rockstar_id);
		};

		if (error && msgType != rage::eNetMessage::MsgJoinResponse)
		{
			LOGF(WARNING, "Error packet that isn't MsgJoinResponse from {}", get_name_rid());
			return true;
		}

		if (g_debug.log_packets)
			log_net_message(msgType, buffer, event, peer);

		switch (msgType)
		{
		case rage::eNetMessage::MsgTextMessage:
		case rage::eNetMessage::MsgTextMessage2:
		{
			if (!plyr)
				return true;

			if (!plyr->whitelist_spammer && plyr->is_spammer)
				return true;

			char message[256];
			buffer.ReadString(message, sizeof(message));

			if (!plyr->whitelist_spammer && chat::is_player_spammer(message, plyr))
			{
				auto n = get_name_rid();
				g_notification_service.push_warning("Chat Spam", n, true);
				g_log.log_additional(std::format("Spam Message - p {}, m {}", n, message));

				plyr->is_spammer = true;

				if (_player)
				{
					plyr->spam_message = message;
					plyr->is_blocked   = true;

					g_blocked_players_service.add_player(plyr, true, true);
					if (g_session.auto_kick_chat_spammers && g_player_service->get_self()->is_host())
						dynamic_cast<player_command*>(command::get("breakup"_J))->call(plyr);
				}

				return true;
			}

			if (g_session.log_chat_messages_to_textbox)
				g_custom_chat_buffer.append_msg(player_name, message);

			break;
		}
		case rage::eNetMessage::MsgScriptMigrateHost:
		{
			if (plyr->block_host_migr_requests)
				return true;

			if (plyr->m_host_migration_rate_limit.process())
			{
				plyr->block_host_migr_requests = true;
				g_reactions.oom_kick2.process(_player ? plyr : nullptr);
				if (_player == nullptr)
					LOGF(WARNING, "OOM kick 2 from unkown - {}", get_name_rid());

				return true;
			}

			break;
		}
		// case rage::eNetMessage::MsgScriptHostRequest:
		// {
		// 	CGameScriptId script;
		// 	script_id_deserialize(script, buffer);
		// 	const char* text = should_block_script_control_request(script.m_hash);

		// 	if (_player && text)
		// 	{
		// 		LOGF(WARNING, "MsgScriptHostRequest for {} denied to {}", text, get_name_rid());
		// 		return true;
		// 	}

		// 	break;
		// }
		case rage::eNetMessage::MsgKickPlayer:
		{
			KickReason reason = buffer.Read<KickReason>(3);
			auto is_host      = is_host_of_session(gta_util::get_network()->m_game_session_ptr, event->m_peer_id);
			auto itr          = kick_reasons.find(reason);

			g_notification_service.push_warning("Kick Player Message",
			    std::format("Received \"{}\" from {} ({})", itr != kick_reasons.end() ? itr->second : "", get_name_rid(), is_host ? "host" : "non-host"),
			    true);

			if (!is_host)
				return true;

			if (reason == KickReason::VOTED_OUT)
			{
				g_session.next_host_list.list.clear();
				if (g_session.num_vote_kicks == 0)
					g_blocked_players_service.add_player(rockstar_id, {player_name, false, false, "host kicked you without any votes"});

				return true;
			}

			break;
		}
		case rage::eNetMessage::MsgRadioStationSyncRequest:
		{
			if (plyr->block_radio_requests)
				return true;

			if (plyr->m_radio_request_rate_limit.process())
			{
				plyr->block_radio_requests = true;
				g_reactions.oom_kick.process(_player ? plyr : nullptr);
				if (_player == nullptr)
					LOGF(WARNING, "OOM kick from unkown - {}", get_name_rid());

				return true;
			}

			if (!session || !session->is_host())
			{
				LOGF(WARNING, "MsgRadioStationSyncRequest from {} (I'm not host)", get_name_rid());
				return true;
			}

			break;
		}
		case rage::eNetMessage::MsgRadioStationSync:
		{
			if (!session || !is_host_of_session(session, event->m_peer_id))
			{
				LOGF(WARNING, "MsgRadioStationSync from {} (They're not host)", get_name_rid());
				return true;
			}
			break;
		}
		case rage::eNetMessage::MsgRequestObjectIds:
		{
			if (!_player)
				break;

			if (!gta_util::get_net_object_ids())
			{
				LOGF(WARNING, "MsgRequestObjectIds from {} (CNetworkObjectMgr is null)", get_name_rid());
				return true;
			}

			if (_player->received_object_id_request)
			{
				LOGF(WARNING, "MsgRequestObjectIds from {} (we've already received request)", get_name_rid());
				return true;
			}

			_player->received_object_id_request = true;
			break;
		}
		case rage::eNetMessage::MsgInformObjectIds:
		{
			if (!_player)
			{
				LOGF(WARNING, "MsgInformObjectIds from {} (they're not physical yet)", get_name_rid());
				return true;
			}

			if (!gta_util::get_net_object_ids())
			{
				LOGF(WARNING, "MsgInformObjectIds from {} (CNetworkObjectMgr is null)", get_name_rid());
				return true;
			}

			if ((gta_util::get_net_object_ids()->m_object_id_response_pending_players & (1 << _player->id())) == 0
			    && _player->received_object_id_response)
			{
				LOGF(WARNING, "MsgInformObjectIds from {} (I didn't request)", get_name_rid());
				return true;
			}

			int num_objects_in_our_range = buffer.Read<int>(13);

			if (num_objects_in_our_range > 256)
			{
				LOGF(WARNING, "MsgInformObjectIds from {} (unusual amount of occupied object IDs)", get_name_rid());
				gta_util::get_net_object_ids()->m_object_id_response_pending_players &= (1 << _player->id());
				return true;
			}

			buffer.SeekForward(num_objects_in_our_range * 13); // we don't really care about this segment

			int num_replacement_objects = buffer.Read<int>(13);

			for (int i = 0; i < num_replacement_objects; i++)
			{
				auto object_id = buffer.Read<std::uint16_t>(13);

				if ((*g_pointers->m_gta.m_network_object_mgr)->find_object_by_id(object_id, true))
				{
					LOGF(WARNING, "MsgInformObjectIds from {} (object is not free)", get_name_rid());
					gta_util::get_net_object_ids()->m_object_id_response_pending_players &= (1 << _player->id());
					return true;
				}
			}

			_player->received_object_id_response = true;
			break;
		}
		case rage::eNetMessage::MsgRoamingJoinBubbleAck:
		{
			int status = buffer.Read<int>(2);
			int bubble = buffer.Read<int>(4);

			if (status == 0 && bubble == 10)
			{
				// LOGF(WARNING, "MsgRoamingJoinBubbleAck from {} (null bubble id)", get_name_rid());
				g_reactions.break_game.process(_player ? _player : nullptr);
				return true;
			}
			else if (status == 0)
			{
				LOGF(WARNING, "MsgRoamingJoinBubbleAck from {} (want me to join them)", get_name_rid(), bubble);
				return true;
			}

			break;
		}
		case rage::eNetMessage::MsgRoamingInitialBubble:
		{
			// should not get this after the host has joined
			if (_player && g_player_service->get_self()->id() != -1)
			{
				LOGF(WARNING, "MsgRoamingInitialBubble from {} (host has joined already!)", get_name_rid());
				return true;
			}

			int my_bubble    = buffer.Read<int>(4);
			int my_pid       = buffer.Read<int>(6);
			int their_bubble = buffer.Read<int>(4);
			int their_pid    = buffer.Read<int>(6);

			if (their_bubble == 10)
			{
				LOGF(WARNING, "MsgRoamingInitialBubble from {} (host doesn't have a bubble?)", get_name_rid());
				return true;
			}

			if (my_bubble == 10)
			{
				LOGF(WARNING, "MsgRoamingInitialBubble from {} (host didn't gave a valid bubble)", get_name_rid());
				return true;
			}

			if (my_bubble > 10 || their_bubble > 10)
			{
				LOGF(WARNING, "MsgRoamingInitialBubble from {} (out of bounds crash by host)", get_name_rid());
				return true;
			}

			if (my_bubble != 0)
				LOGF(WARNING, "MsgRoamingInitialBubble from {} (non-standard bubble id: {})", get_name_rid(), my_bubble);

			if (my_bubble != their_bubble)
			{
				LOGF(WARNING, "MsgRoamingInitialBubble from {} (host bubble != mine, {} != {})", get_name_rid(), their_bubble, my_bubble);
				return true;
			}

			if (my_pid >= 32 || their_pid >= 32)
				LOGF(WARNING, "MsgRoamingInitialBubble from {} (host sent invalid player ids)", get_name_rid());

			if (my_pid == their_pid)
			{
				LOGF(WARNING, "MsgRoamingInitialBubble from {} (host pid == mine)", get_name_rid());
				return true;
			}

			break;
		}
		case rage::eNetMessage::MsgNonPhysicalData:
		{
			buffer.Read<int>(7); // size
			int bubble_id = buffer.Read<int>(4);
			int player_id = buffer.Read<int>(6);

			if (_player)
				return true; // we don't need this message anymore

			if (bubble_id == 10)
			{
				LOGF(WARNING, "MsgNonPhysicalData from {} (they're not in a bubble)", get_name_rid());
				return true; // might as well drop it
			}

			if (bubble_id > 10)
			{
				LOGF(WARNING, "MsgNonPhysicalData from {} (out of bounds crash)", get_name_rid());
				return true;
			}

			if (bubble_id != 0)
				LOGF(WARNING, "MsgNonPhysicalData from {} (non-standard bubble id: {})", get_name_rid(), bubble_id);

			if (player_id >= 32)
			{
				LOGF(WARNING, "MsgNonPhysicalData from {} (invalid player id)", get_name_rid());
				return true;
			}

			if (g_player_service->get_self() && g_player_service->get_self()->id() != -1 && g_player_service->get_self()->id() == player_id)
			{
				LOGF(WARNING, "MsgNonPhysicalData from {} (trying to replace me)", get_name_rid());
				return true;
			}

			for (auto& player : g_player_service->players())
				if (player.second->id() == player_id)
				{
					LOGF(WARNING, "MsgNonPhysicalData from {} (trying to replace {})", get_name_rid(), player.second->m_name);
					return true;
				}

			break;
		}
		case rage::eNetMessage::MsgRequestKickFromHost:
		{
			LOGF(WARNING, "Denied MsgRequestKickFromHost from {}", get_name_rid());
			return true;
		}
		case rage::eNetMessage::MsgConfigRequest:
		{
			LOGF(WARNING, "Denied MsgConfigRequest from {}", get_name_rid());
			return true;
		}
		// case rage::eNetMessage::MsgScriptJoin:
		// {
		// 	CGameScriptId script;
		// 	script_id_deserialize(script, buffer);
		// 	const char* text = should_block_script_control_request(script.m_hash);

		// 	if (_player && text)
		// 	{
		// 		packet pkt;
		// 		pkt.write_message(rage::eNetMessage::MsgScriptJoinHostAck);
		// 		script_id_serialize(script, pkt.m_buffer);
		// 		pkt.write<int16_t>(-1, 16);
		// 		pkt.write<int16_t>(-1, 16);
		// 		pkt.write<bool>(false, 1);
		// 		pkt.write<int>(2, 3);
		// 		pkt.send(event->m_msg_id);

		// 		LOGF(WARNING, "MsgScriptJoin for {} denied to {}", text, get_name_rid());
		// 		return true;
		// 	}

		// 	break;
		// }
		default:
		{
			if ((int)msgType > 0x91)
			{
				LOGF(WARNING, "{} sent a message that does not exist: {:X}", get_name_rid(), (int)msgType);

				// dumb modders
				if (_player)
					g_reactions.sent_modder_beacons.process(_player);
				else
					g_player_service->mark_player_as_sending_modder_beacons(rockstar_id);
			}
		}
		}

		return g_hooking->get_original<hooks::receive_net_message>()(a1, net_cxn_mgr, event);
	}
}
