#include "core/data/reactions.hpp"
#include "core/vars.hpp"
#include "hooking/hooking.hpp"
#include "pointers.hpp"
#include "services/players/player_service.hpp"

namespace big
{
	void hooks::received_clone_remove(CNetworkObjectMgr* mgr, CNetGamePlayer* src, CNetGamePlayer* dst, int16_t object_id, uint32_t ownership_token)
	{
		auto plyr = g_player_service->get_by_id(src->m_player_id);

		if (dst && dst->m_player_info && dst->m_player_info->m_ped && dst->m_player_info->m_ped->m_net_object
		    && object_id == dst->m_player_info->m_ped->m_net_object->m_object_id)
		{
			g_reactions.player_ped_remove.process(plyr);
			return;
		}

		if (g_local_player && g_local_player->m_vehicle && g_local_player->m_vehicle->m_net_object
		    && object_id == g_local_player->m_vehicle->m_net_object->m_object_id)
			g_reactions.delete_vehicle_cr.process(plyr);

		if (auto object = g_pointers->m_gta.m_get_net_object(*g_pointers->m_gta.m_network_object_mgr, object_id, true))
			if (object->m_object_type == (int)eNetObjType::NET_OBJ_TYPE_PLAYER && object->m_owner_id != src->m_player_id)
			{
				auto tgt = g_player_service->get_by_id(object->m_owner_id);
				LOGF(WARNING, "{} is trying to delete {}'s player ped", src->get_name(), tgt ? tgt->m_name : "<UNKNOWN>");
				return;
			}

		g_hooking->get_original<hooks::received_clone_remove>()(mgr, src, dst, object_id, ownership_token);
	}
}