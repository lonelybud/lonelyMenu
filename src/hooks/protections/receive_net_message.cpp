#include "backend/command.hpp"
#include "backend/player_command.hpp"
#include "core/data/kick_reasons.hpp"
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

static inline void gamer_handle_deserialize(rage::rlGamerHandle& hnd, rage::datBitBuffer& buf)
{
	constexpr int PC_PLATFORM = 3;
	if ((hnd.m_platform = buf.Read<uint8_t>(8)) != PC_PLATFORM)
		return;

	buf.ReadInt64((int64_t*)&hnd.m_rockstar_id, 64);
	hnd.unk_0009 = buf.Read<uint8_t>(8);
}

static inline bool is_kick_instruction(rage::datBitBuffer& buffer)
{
	rage::rlGamerHandle sender, receiver;
	char name[18];
	gamer_handle_deserialize(sender, buffer);
	gamer_handle_deserialize(receiver, buffer);
	buffer.ReadString(name, 17);
	int instruction = buffer.Read<int>(32);
	return instruction == 8;
}

namespace big
{
	static inline bool has_invalid_time_bw_msgs(big::player_ptr player)
	{
		auto currentTime      = std::chrono::system_clock::now();
		auto diff             = std::chrono::duration_cast<std::chrono::seconds>(currentTime - player->last_msg_time);
		player->last_msg_time = currentTime;

		if (diff.count() <= 2.5)
			return true;
		return false;
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
					if (sn_player->m_player_data.m_host_token)
						player = g_player_service->get_by_host_token(sn_player->m_player_data.m_host_token);
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
				buffer.ReadString(message, 256);

				if (strlen(message) > 55 && has_invalid_time_bw_msgs(player))
				{
					LOG(WARNING) << player->get_name() << " seem to spam chat message.";
					// flag as spammer
					player->is_spammer   = true;
					player->spam_message = message;
					player->is_blocked   = true;

					g_fiber_pool->queue_job([player] {
						bad_players_nm::add_player(player, true, true);
						if (g_session.auto_kick_chat_spammers && g_player_service->get_self()->is_host())
							dynamic_cast<player_command*>(command::get(RAGE_JOAAT("hostkick")))->call(player);
					});

					return true;
				}

				if (g_session.log_chat_messages_to_textbox)
					g_custom_chat_buffer.append_msg(player->get_name(), message);

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

				if (g_session.force_script_host)
					switch (script.m_hash)
					{
					case RAGE_JOAAT("freemode"):
					case RAGE_JOAAT("fmmc_launcher"):
					case RAGE_JOAAT("am_launcher"): return true;
					}

				break;
			}
			case rage::eNetMessage::MsgKickPlayer:
			{
				KickReason reason = buffer.Read<KickReason>(3);

				if (auto itr = kick_reasons.find(reason); itr != kick_reasons.end())
					g_notification_service->push_warning("Kick Player Message",
					    std::format("Received \"{}\" from {} ({})", itr->second, player->get_name(), player->is_host() ? "host" : "non-host"),
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
			case rage::eNetMessage::MsgScriptMigrateHost: return true;
			case rage::eNetMessage::MsgRadioStationSyncRequest: return true;
			}
		}

		return g_hooking->get_original<hooks::receive_net_message>()(netConnectionManager, a2, frame);
	}
}
