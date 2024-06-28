#include "core/data/reactions.hpp"
#include "core/data/syncing_player.hpp"
#include "hooking/hooking.hpp"
#include "logger/logger.hpp"
#include "pointers.hpp"
#include "services/players/player_service.hpp"

namespace big
{
	eAckCode hooks::received_clone_sync(CNetworkObjectMgr* mgr, CNetGamePlayer* src, CNetGamePlayer* dst, eNetObjType object_type, uint16_t object_id, rage::datBitBuffer* buffer, uint16_t unk, uint32_t timestamp)
	{
		auto plyr = g_player_service->get_by_id(src->m_player_id);

		if (object_type < eNetObjType::NET_OBJ_TYPE_AUTOMOBILE || object_type > eNetObjType::NET_OBJ_TYPE_TRAIN)
		{
			g_reactions.crash29.process(plyr);
			return eAckCode::ACKCODE_FAIL;
		}

		if (auto net_obj = g_pointers->m_gta.m_get_net_object(mgr, object_id, true); net_obj && net_obj->m_object_type != (int16_t)object_type)
		{
			g_reactions.crash30.process(plyr);
			return eAckCode::ACKCODE_FAIL;
		}

		// can be used to crash or reverse sync player data
		if (dst && dst->m_player_info && dst->m_player_info->m_ped && dst->m_player_info->m_ped->m_net_object
		    && object_id == dst->m_player_info->m_ped->m_net_object->m_object_id)
		{
			g_reactions.player_ped_sync.process(plyr);
			return eAckCode::ACKCODE_FAIL;
		}

		if (auto object = g_pointers->m_gta.m_get_net_object(*g_pointers->m_gta.m_network_object_mgr, object_id, true))
			if (object->m_object_type == (int)eNetObjType::NET_OBJ_TYPE_PLAYER && object->m_owner_id != src->m_player_id)
			{
				auto tgt = g_player_service->get_by_id(object->m_owner_id);
				LOGF(WARNING, "{} is trying to sync to {}'s player ped", src->get_name(), tgt ? tgt->m_name : "<UNKNOWN>");
				return eAckCode::ACKCODE_FAIL;
			}

		if (plyr && plyr->block_clone_sync)
			return eAckCode::ACKCODE_FAIL;

		m_syncing_player      = src;
		m_syncing_object_type = object_type;

		return g_hooking->get_original<received_clone_sync>()(mgr, src, dst, object_type, object_id, buffer, unk, timestamp);
	}
}
