#include "core/data/syncing_player.hpp"
#include "hooking.hpp"
#include "services/players/player_service.hpp"
#include "util/notify.hpp"

namespace big
{
	void hooks::received_clone_create(CNetworkObjectMgr* mgr, CNetGamePlayer* src, CNetGamePlayer* dst, eNetObjType object_type, int32_t object_id, int32_t object_flag, rage::datBitBuffer* buffer, int32_t timestamp)
	{
		auto plyr = g_player_service->get_by_id(src->m_player_id);

		if (object_type < eNetObjType::NET_OBJ_TYPE_AUTOMOBILE || object_type > eNetObjType::NET_OBJ_TYPE_TRAIN)
		{
			notify::crash_blocked(plyr, 29);
			return;
		}

		if (plyr && plyr->block_clone_create)
			return;

		m_syncing_player      = src;
		m_syncing_object_type = object_type;

		g_hooking->get_original<hooks::received_clone_create>()(mgr, src, dst, object_type, object_id, object_flag, buffer, timestamp);
	}
}