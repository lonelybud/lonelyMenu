#include "core/data/context_menu.hpp"
#include "core/data/debug.hpp"
#include "core/data/desync_kick.hpp"
#include "core/data/misc.hpp"
#include "core/data/self.hpp"
#include "core/data/session.hpp"
#include "core/data/syncing_player.hpp"
#include "core/data/unknown_player.hpp"
#include "core/data/vehicle.hpp"
#include "core/data/weapons.hpp"
#include "core/settings/esp.hpp"
#include "hooking/hooking.hpp"
#include "logger/logger.hpp"
#include "services/players/player_service.hpp"

#include <network/CNetworkPlayerMgr.hpp>

namespace big
{
	static void clean_up()
	{
		g_self                = {};
		g_vehicle             = {};
		g_weapons             = {};
		g_context_menu        = {};
		g_misc                = {};
		g_debug               = {};
		g_unknown_players     = {};
		g_desync_kick_players = {};

		g_session_t g_session_temp;
		g_session_temp.host_token                 = g_session.host_token;
		g_session_temp.orig_host_token            = g_session.orig_host_token;
		g_session_temp.orig_peer_id               = g_session.orig_peer_id;
		g_session_temp.nat_type                   = g_session.nat_type;
		g_session_temp.multiplex_session          = g_session.multiplex_session;
		g_session_temp.multiplex_count            = g_session.multiplex_count;
		g_session_temp.spoof_session_region_type  = g_session.spoof_session_region_type;
		g_session_temp.session_region_type        = g_session.session_region_type;
		g_session_temp.spoof_session_language     = g_session.spoof_session_language;
		g_session_temp.session_language           = g_session.session_language;
		g_session_temp.spoof_session_player_count = g_session.spoof_session_player_count;
		g_session_temp.session_player_count       = g_session.session_player_count;
		g_session                                 = g_session_temp;

		g_esp_t g_esp_temp;
		g_esp.enabled = g_esp_temp.enabled;
		g_esp.enabled = g_esp_temp.distance;

		m_syncing_player      = nullptr;
		m_syncing_object_type = (eNetObjType)-1;
	}

	bool hooks::network_player_mgr_init(CNetworkPlayerMgr* _this, uint64_t a2, uint32_t a3, uint32_t a4[4])
	{
		LOG(INFO) << "CNetworkPlayerMgr init";

		bool result = g_hooking->get_original<hooks::network_player_mgr_init>()(_this, a2, a3, a4);

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
