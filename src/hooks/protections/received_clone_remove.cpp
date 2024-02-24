#include "core/data/reactions.hpp"
#include "hooking/hooking.hpp"
#include "services/players/player_service.hpp"
#include "util/notify.hpp"

namespace big
{
	void hooks::received_clone_remove(CNetworkObjectMgr* mgr, CNetGamePlayer* src, CNetGamePlayer* dst, int16_t object_id, uint32_t ownership_token)
	{
		if (dst->m_player_info && dst->m_player_info->m_ped && dst->m_player_info->m_ped->m_net_object
		    && dst->m_player_info->m_ped->m_net_object->m_object_id == object_id)
		{
			if (auto plyr = g_player_service->get_by_id(src->m_player_id))
				g_reactions.player_ped_removal.process(plyr);

			return;
		}
		else
			g_log.log_additional(std::format("Player ped removal from {} to {}", src->get_name(), dst->get_name()));

		g_hooking->get_original<hooks::received_clone_remove>()(mgr, src, dst, object_id, ownership_token);
	}
}