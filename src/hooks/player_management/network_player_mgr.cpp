#include "core/data/self.hpp"
#include "core/data/session.hpp"
#include "core/data/syncing_player.hpp"
#include "core/data/vehicle.hpp"
#include "core/data/weapons.hpp"
#include "core/settings/esp.hpp"
#include "hooking.hpp"
#include "services/players/player_service.hpp"

#include <network/CNetworkPlayerMgr.hpp>

namespace big
{
	bool hooks::network_player_mgr_init(CNetworkPlayerMgr* _this, uint64_t a2, uint32_t a3, uint32_t a4[4])
	{
		LOG(INFO) << "CNetworkPlayerMgr#init got called, we're probably entering a session.";

		bool result = g_hooking->get_original<hooks::network_player_mgr_init>()(_this, a2, a3, a4);

		g_player_service->player_join(_this->m_local_net_player);

		return result;
	}

	void hooks::network_player_mgr_shutdown(CNetworkPlayerMgr* _this)
	{
		LOG(INFO) << "CNetworkPlayerMgr#shutdown got called, we're probably leaving our session.";

		g_player_service->do_cleanup();

		g_self    = {};
		g_vehicle = {};

		g_weapons_t g_weapons_temp;
		g_weapons.aimbot.enable = g_weapons_temp.aimbot.enable;

		g_session_t g_session_temp;
		g_session.lock_session                 = g_session_temp.lock_session;
		g_session.next_host_list.list          = g_session_temp.next_host_list.list;
		g_session.log_chat_messages_to_textbox = g_session_temp.log_chat_messages_to_textbox;
		g_session.decloak_players              = g_session_temp.decloak_players;
		g_session.custom_time                  = g_session_temp.custom_time;

		g_esp_t g_esp_temp;
		g_esp.enabled             = g_esp_temp.enabled;
		g_esp.show_player         = g_esp_temp.show_player;
		g_esp.show_gs_cache_boxes = g_esp_temp.show_gs_cache_boxes;

		m_syncing_player      = nullptr;
		m_syncing_object_type = (eNetObjType)-1;

		g_hooking->get_original<hooks::network_player_mgr_shutdown>()(_this);
	}
}
