#include "core/data/self.hpp"
#include "core/data/session.hpp"
#include "core/data/vehicle.hpp"
#include "core/data/weapons.hpp"
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

		g_self.unlimited_oxygen = false;
		g_self.god_mode         = false;

		g_vehicle.god_mode  = false;
		g_vehicle.veh_boost = false;
		g_vehicle.seatbelt  = false;

		g_weapons.aimbot.enable = false;

		g_session.lock_session = false;
		g_session.next_host_list.list.clear();

		g_hooking->get_original<hooks::network_player_mgr_shutdown>()(_this);
	}
}
