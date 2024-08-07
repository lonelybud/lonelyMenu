#include "core/data/region_codes.hpp"
#include "core/data/session.hpp"
#include "hooking/hooking.hpp"
#include "logger/logger.hpp"

#include <network/CMsgJoinRequest.hpp>
#include <network/CNetGamePlayerDataMsg.hpp>
#include <network/RemoteGamerInfoMsg.hpp>

namespace big
{
	bool hooks::serialize_join_request_message(RemoteGamerInfoMsg* info, void* data, int size, int* bits_serialized)
	{
		std::string str = "serialize_join_request_message";

		if (g_session.nat_type != 4)
		{
			info->m_gamer_info.m_nat_type = g_session.nat_type;
			str += " using nattype: ";
			str += nat_types[g_session.nat_type].name;
		}

		LOG(INFO) << str;

		return g_hooking->get_original<hooks::serialize_join_request_message>()(info, data, size, bits_serialized);
	}

	bool hooks::serialize_join_request_message_2(CMsgJoinRequest* msg, void* buf, int size, int* bits_serialized)
	{
		auto& data      = msg->m_player_data_msg;
		std::string str = "serialize_join_request_message 2: ";
		str += regions[data.m_region].name;

		if (g_session.nat_type != 4)
		{
			data.m_nat_type = g_session.nat_type;
			str += " using nattype: ";
			str += nat_types[g_session.nat_type].name;
		}

		LOG(INFO) << str;

		return g_hooking->get_original<hooks::serialize_join_request_message_2>()(msg, buf, size, bits_serialized);
	}
}