#include "core/data/reactions.hpp"
#include "core/data/syncing_player.hpp"
#include "core/vars.hpp"
#include "gta/pools.hpp"
#include "gta_util.hpp"
#include "hooking/hooking.hpp"
#include "logger/logger.hpp"
#include "services/players/player_service.hpp"

#include <network/netObjectIds.hpp>

namespace big
{
	void hooks::received_clone_create(CNetworkObjectMgr* mgr, CNetGamePlayer* src, CNetGamePlayer* dst, eNetObjType object_type, int32_t object_id, int32_t object_flag, rage::datBitBuffer* buffer, int32_t timestamp)
	{
		auto plyr = g_player_service->get_by_id(src->m_player_id);

		if (object_type < eNetObjType::NET_OBJ_TYPE_AUTOMOBILE || object_type > eNetObjType::NET_OBJ_TYPE_TRAIN)
		{
			g_reactions.crash29.process(plyr);
			return;
		}

		// can delete objects here too
		if (dst && dst->m_player_info && dst->m_player_info->m_ped && dst->m_player_info->m_ped->m_net_object
		    && object_id == dst->m_player_info->m_ped->m_net_object->m_object_id)
		{
			g_reactions.player_ped_create.process(plyr);
			return;
		}

		if (g_local_player && g_local_player->m_vehicle && g_local_player->m_vehicle->m_net_object
		    && object_id == g_local_player->m_vehicle->m_net_object->m_object_id)
			g_reactions.delete_vehicle_cc.process(plyr);

		if (auto object = g_pointers->m_gta.m_get_net_object(*g_pointers->m_gta.m_network_object_mgr, object_id, true))
			if (object->m_object_type == (int)eNetObjType::NET_OBJ_TYPE_PLAYER && object->m_owner_id != src->m_player_id)
			{
				auto tgt = g_player_service->get_by_id(object->m_owner_id);
				LOGF(WARNING, "{} is trying to delete {}'s player ped", src->get_name(), tgt ? tgt->m_name : "<UNKNOWN>");
				return;
			}

		switch (object_type)
		{
		case eNetObjType::NET_OBJ_TYPE_AUTOMOBILE:
		case eNetObjType::NET_OBJ_TYPE_BIKE:
		case eNetObjType::NET_OBJ_TYPE_BOAT:
		case eNetObjType::NET_OBJ_TYPE_HELI:
		case eNetObjType::NET_OBJ_TYPE_PLANE:
		case eNetObjType::NET_OBJ_TYPE_SUBMARINE:
		case eNetObjType::NET_OBJ_TYPE_TRAILER:
		case eNetObjType::NET_OBJ_TYPE_TRAIN:
			if ((*g_pointers->m_gta.m_vehicle_allocator)->m_size < 10)
			{
				// We don't have enough memory to handle this
				LOG(WARNING) << "received_clone_create low pool size: " << src->get_name();
				return;
			}
		}

		if (gta_util::get_net_object_ids()->is_object_id_usable(object_id))
		{
			// LOGF(WARNING, "{} sent us an object create request with an object ID that is in our usable object ID list. Somebody lied to us...", src->get_name());
			gta_util::get_net_object_ids()->remove_object_id(object_id);
		}

		if (plyr && plyr->block_clone_create)
			return;

		m_syncing_player      = src;
		m_syncing_object_type = object_type;

		g_hooking->get_original<hooks::received_clone_create>()(mgr, src, dst, object_type, object_id, object_flag, buffer, timestamp);
	}
}