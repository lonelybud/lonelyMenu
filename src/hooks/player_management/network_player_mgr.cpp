#include "core/data/context_menu.hpp"
#include "core/data/debug.hpp"
#include "core/data/misc.hpp"
#include "core/data/self.hpp"
#include "core/data/session.hpp"
#include "core/data/syncing_player.hpp"
#include "core/data/vehicle.hpp"
#include "core/data/weapons.hpp"
#include "core/settings/esp.hpp"
#include "hooking/hooking.hpp"
#include "services/players/player_service.hpp"

#include <network/CNetworkPlayerMgr.hpp>

namespace big
{
	static void clean_up()
	{
		g_self         = {};
		g_vehicle      = {};
		g_weapons      = {};
		g_context_menu = {};
		g_misc         = {};
		g_debug        = {};

		g_session_t g_session_temp;
		g_session.lock_session                 = g_session_temp.lock_session;
		g_session.next_host_list               = {};
		g_session.log_chat_messages_to_textbox = g_session_temp.log_chat_messages_to_textbox;
		g_session.decloak_players              = g_session_temp.decloak_players;
		g_session.auto_kick_chat_spammers      = g_session_temp.auto_kick_chat_spammers;
		g_session.notified_as_host             = g_session_temp.notified_as_host;

		g_esp_t g_esp_temp;
		g_esp.enabled = g_esp_temp.enabled;

		m_syncing_player      = nullptr;
		m_syncing_object_type = (eNetObjType)-1;
	}

	bool hooks::network_player_mgr_init(CNetworkPlayerMgr* _this, uint64_t a2, uint32_t a3, uint32_t a4[4])
	{
		LOG(INFO) << "CNetworkPlayerMgr init";

		bool result = g_hooking->get_original<hooks::network_player_mgr_init>()(_this, a2, a3, a4);

		clean_up();

		return result;
	}

	void hooks::network_player_mgr_shutdown(CNetworkPlayerMgr* _this)
	{
		LOG(INFO) << "CNetworkPlayerMgr shutdown";

		g_player_service->do_cleanup();

		clean_up();

		g_hooking->get_original<hooks::network_player_mgr_shutdown>()(_this);
	}
}
