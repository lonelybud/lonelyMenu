#include "backend/command.hpp"
#include "backend/player_command.hpp"
#include "core/data/debug.hpp"
#include "core/data/kick_reasons.hpp"
#include "core/data/packet_types.hpp"
#include "core/data/reactions.hpp"
#include "core/data/session.hpp"
#include "file_manager/file.hpp"
#include "gta/net_game_event.hpp"
#include "gta_util.hpp"
#include "hooking/hooking.hpp"
#include "natives.hpp"
#include "script/scriptIdBase.hpp"
#include "services/bad_players/bad_players.hpp"
#include "services/custom_chat_buffer.hpp"
#include "services/notifications/notification_service.hpp"
#include "services/players/player_service.hpp"

#include <cstdlib>

namespace big
{
	static inline bool is_spam_interval_diff_there(char* msg, big::player_ptr player, std::chrono::seconds diff, int limit, int type)
	{
		if (abs(player->last_spam_interval_diff - diff).count() <= 1 // the diff bw arrival of last and this message is atmost 1 sec
		    && (type == 0                                            // type is 0
		        || diff.count() <= 5 // it should take atleast these seconds to receive when type is 1
		        ))
		{
			if (type == 1)
			{
				if (++player->same_interval_spam_count_high == limit)
				{
					g_log.log_additional(std::format("Chat Spammer - p {}, i1 {}, i2 {}, t {}, c {}",
					    player->m_name,
					    player->last_spam_interval_diff,
					    diff,
					    1,
					    player->same_interval_spam_count_high));
					return true;
				}
			}
			else
			{
				if (++player->same_interval_spam_count_low == limit)
				{
					g_log.log_additional(std::format("Chat Spammer - p {}, i1 {}, i2 {}, t {}, c {}",
					    player->m_name,
					    player->last_spam_interval_diff,
					    diff,
					    0,
					    player->same_interval_spam_count_low));
					return true;
				}
			}
		}
		else
		{
			player->same_interval_spam_count_low  = 0;
			player->same_interval_spam_count_high = 0;
		}

		player->last_spam_interval_diff = diff;
		return false;
	}

	static inline bool is_player_spammer(char* msg, big::player_ptr player)
	{
		auto currentTime = std::chrono::system_clock::now();
		auto diff        = std::chrono::duration_cast<std::chrono::seconds>(currentTime - player->last_msg_time);

		// first message should be allowed
		if (player->last_msg_time == std::chrono::system_clock::time_point::min())
		{
			player->last_msg_time = currentTime;
			return false;
		}
		player->last_msg_time = currentTime;

		if (strlen(msg) > 50)
		{
			// it should take atleast 2 seconds: to type or copy and paste
			if (diff.count() <= 2)
				return true;

			return is_spam_interval_diff_there(msg, player, diff, 3, 1);
		}

		return is_spam_interval_diff_there(msg, player, diff, 15, 0);
	}

	bool get_msg_type(rage::eNetMessage& msgType, rage::datBitBuffer& buffer)
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

	static void script_id_deserialize(CGameScriptId& id, rage::datBitBuffer& buffer)
	{
		id.m_hash      = buffer.Read<uint32_t>(32);
		id.m_timestamp = buffer.Read<uint32_t>(32);

		if (buffer.Read<bool>(1))
			id.m_position_hash = buffer.Read<uint32_t>(32);

		if (buffer.Read<bool>(1))
			id.m_instance_id = buffer.Read<int32_t>(8);
	}

	bool hooks::receive_net_message(void* netConnectionManager, void* a2, rage::netConnection::InFrame* frame)
	{
		if (frame->get_event_type() != rage::netConnection::InFrame::EventType::FrameReceived)
			return g_hooking->get_original<hooks::receive_net_message>()(netConnectionManager, a2, frame);

		if (frame->m_data == nullptr || frame->m_length == 0)
			return g_hooking->get_original<hooks::receive_net_message>()(netConnectionManager, a2, frame);

		rage::datBitBuffer buffer(frame->m_data, frame->m_length);
		buffer.m_flagBits = 1;

		rage::eNetMessage msgType;

		if (!get_msg_type(msgType, buffer))
			return g_hooking->get_original<hooks::receive_net_message>()(netConnectionManager, a2, frame);

		player_ptr player;

		for (uint32_t i = 0; i < gta_util::get_network()->m_game_session_ptr->m_player_count; i++)
			if (auto sn_player = gta_util::get_network()->m_game_session_ptr->m_players[i])
				if (sn_player && sn_player->m_player_data.m_peer_id_2 == frame->m_peer_id)
				{
					player = g_player_service->get_by_host_token(sn_player->m_player_data.m_host_token);

					if (!player)
						player = g_player_service->get_by_name(sn_player->m_player_data.m_name);
					break;
				}

		if (player)
		{
			switch (msgType)
			{
			case rage::eNetMessage::MsgTextMessage:
			case rage::eNetMessage::MsgTextMessage2:
			{
				if (player->is_spammer)
					return true;

				char message[256];
				buffer.ReadString(message, sizeof(message));

				if (!player->whitelist_spammer && is_player_spammer(message, player))
				{
					LOG(WARNING) << player->m_name << " seem to spam chat message.";
					g_log.log_additional(std::format("Spam Message - p {}, m {}", player->m_name, message));

					// flag as spammer
					player->is_spammer   = true;
					player->spam_message = message;
					player->is_blocked   = true;

					g_bad_players_service.add_player(player, true, true);
					if (g_session.auto_kick_chat_spammers && g_player_service->get_self()->is_host())
						dynamic_cast<player_command*>(command::get("hostkick"_J))->call(player);

					return true;
				}

				if (g_session.log_chat_messages_to_textbox)
					g_custom_chat_buffer.append_msg(player->m_name, message);

				break;
			}
			case rage::eNetMessage::MsgScriptMigrateHost:
			{
				if (player->block_host_migr_requests)
					return true;

				if (player->m_host_migration_rate_limit.process())
				{
					if (player->m_host_migration_rate_limit.exceeded_last_process())
					{
						player->block_host_migr_requests = true;
						g_reactions.oom_kick2.process(player);
					}
					return true;
				}

				break;
			}
			case rage::eNetMessage::MsgScriptHostRequest:
			{
				CGameScriptId script;
				script_id_deserialize(script, buffer);

				switch (script.m_hash)
				{
				case "freemode"_J:
					if (g_session.force_freemode_host)
						return true;
					break;
				case "fmmc_launcher"_J:
					if (g_session.force_fmmc_launcher_host)
						return true;
					break;
				case "am_launcher"_J:
					if (g_session.force_am_launcher_host)
						return true;
					break;
				}

				break;
			}
			case rage::eNetMessage::MsgKickPlayer:
			{
				KickReason reason = buffer.Read<KickReason>(3);

				if (auto itr = kick_reasons.find(reason); itr != kick_reasons.end())
					g_notification_service.push_warning("Kick Player Message",
					    std::format("Received \"{}\" from {} ({})", itr->second, player->m_name, player->is_host() ? "host" : "non-host"),
					    true);

				if (!player->is_host())
					return true;

				if (reason == KickReason::VOTED_OUT)
					return true;

				break;
			}
			case rage::eNetMessage::MsgRadioStationSyncRequest:
			{
				if (player->block_radio_requests)
					return true;

				if (player->m_radio_request_rate_limit.process())
				{
					if (player->m_radio_request_rate_limit.exceeded_last_process())
					{
						player->block_radio_requests = true;
						g_reactions.oom_kick.process(player);
					}
					return true;
				}

				break;
			}
			}
		}
		else
		{
			switch (msgType)
			{
			case rage::eNetMessage::MsgTextMessage:
			case rage::eNetMessage::MsgTextMessage2:
			{
				if (g_session.log_chat_messages_to_textbox)
				{
					char message[256];
					buffer.ReadString(message, 256);
					g_custom_chat_buffer.append_msg("?", message);
				}

				break;
			}
			case rage::eNetMessage::MsgScriptMigrateHost: return true;
			case rage::eNetMessage::MsgRadioStationSyncRequest: return true;
			}
		}


		if (g_debug.log_packets && msgType != rage::eNetMessage::MsgCloneSync && msgType != rage::eNetMessage::MsgPackedCloneSyncACKs && msgType != rage::eNetMessage::MsgPackedEvents && msgType != rage::eNetMessage::MsgPackedReliables && msgType != rage::eNetMessage::MsgPackedEventReliablesMsgs && msgType != rage::eNetMessage::MsgNetArrayMgrUpdate && msgType != rage::eNetMessage::MsgNetArrayMgrSplitUpdateAck && msgType != rage::eNetMessage::MsgNetArrayMgrUpdateAck && msgType != rage::eNetMessage::MsgScriptHandshakeAck && msgType != rage::eNetMessage::MsgScriptHandshake && msgType != rage::eNetMessage::MsgScriptJoin && msgType != rage::eNetMessage::MsgScriptJoinAck && msgType != rage::eNetMessage::MsgScriptJoinHostAck && msgType != rage::eNetMessage::MsgRequestObjectIds && msgType != rage::eNetMessage::MsgInformObjectIds && msgType != rage::eNetMessage::MsgNetTimeSync)
		{
			const char* packet_type = "<UNKNOWN>";
			for (const auto& p : packet_types)
				if (p.second == (int)msgType)
				{
					packet_type = p.first;
					break;
				}

			g_log.log_additional(std::format("RECEIVED PACKET | Type: {} | Length: {} | Sender: {} | {}",
			    packet_type,
			    frame->m_length,
			    (player ? player->m_name : "?"),
			    (int)msgType));
		}

		return g_hooking->get_original<hooks::receive_net_message>()(netConnectionManager, a2, frame);
	}
}
