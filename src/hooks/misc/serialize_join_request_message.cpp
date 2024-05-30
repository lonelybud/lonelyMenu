#include "core/data/region_codes.hpp"
#include "core/data/session.hpp"
#include "hooking/hooking.hpp"
#include "services/players/player_service.hpp"

#include <network/CNetGamePlayerDataMsg.hpp>
#include <network/RemoteGamerInfoMsg.hpp>

namespace big
{
	bool hooks::serialize_join_request_message(RemoteGamerInfoMsg* info, void* data, int size, int* bits_serialized)
	{
		LOG(INFO) << "serialize_join_request_message";

		if (g_session.nat_type != 4)
		{
			info->m_gamer_info.m_nat_type = g_session.nat_type;
			LOG(INFO) << "using nattype: " << nat_types[g_session.nat_type].name;
		}

		return g_hooking->get_original<hooks::serialize_join_request_message>()(info, data, size, bits_serialized);
	}

	bool hooks::serialize_join_request_message_2(__int64 msg, void* buf, int size, int* bits_serialized)
	{
		auto& data = *(CNetGamePlayerDataMsg*)(msg + 0x128);

		LOG(INFO) << "serialize_join_request_message 2: " << regions[data.m_region].name;

		if (g_session.nat_type != 4)
		{
			data.m_nat_type = g_session.nat_type;
			LOG(INFO) << "using nattype: " << nat_types[g_session.nat_type].name;
		}

		return g_hooking->get_original<hooks::serialize_join_request_message_2>()(msg, buf, size, bits_serialized);
	}
}