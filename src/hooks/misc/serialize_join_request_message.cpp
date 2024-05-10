#include "core/data/protections.hpp"
#include "core/data/region_codes.hpp"
#include "hooking/hooking.hpp"

#include <network/CNetGamePlayerDataMsg.hpp>
#include <network/RemoteGamerInfoMsg.hpp>

namespace big
{
	bool hooks::serialize_join_request_message(RemoteGamerInfoMsg* info, void* data, int size, int* bits_serialized)
	{
		LOG(INFO) << "serialize_join_request_message: " << info->m_gamer_info.m_name << " "
		          << info->m_gamer_info.m_gamer_handle.m_rockstar_id;

		if (g_protections.desync_kick)
			info->m_gamer_info.m_nat_type = 0;

		return g_hooking->get_original<hooks::serialize_join_request_message>()(info, data, size, bits_serialized);
	}

	bool hooks::serialize_join_request_message_2(__int64 msg, void* buf, int size, int* bits_serialized)
	{
		auto& data = *(CNetGamePlayerDataMsg*)(msg + 0x128);

		LOG(INFO) << "serialize_join_request_message 2: " << data.m_account_id << " " << regions[data.m_region].name;

		if (g_protections.desync_kick)
			data.m_nat_type = 0;

		return g_hooking->get_original<hooks::serialize_join_request_message_2>()(msg, buf, size, bits_serialized);
	}
}