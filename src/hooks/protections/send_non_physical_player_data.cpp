#include "hooking.hpp"
#include "pointers.hpp"
#include "services/notifications/notification_service.hpp"
#include "services/recent_modders/recent_modders.hpp"

#include <network/CNetGamePlayer.hpp>
#include <player/CNonPhysicalPlayerData.hpp>

namespace big
{
	bool hooks::send_non_physical_player_data(CNetGamePlayer* player, __int64 message, int flags, void* a4, CNetGamePlayer* a5)
	{
		// auto data         = *(CNonPhysicalPlayerData**)(message + 0x10);
		// int old_bubble_id = data->m_bubble_id;

		// const auto* net_player_data = player->get_net_data();
		// auto rockstar_id            = net_player_data ? net_player_data->m_gamer_handle.m_rockstar_id : 0;
		// auto player_name            = net_player_data ? net_player_data->m_name : "";

		// if (net_player_data && recent_modders_nm::is_blocked(rockstar_id) && *g_pointers->m_gta.m_is_session_started)
		// {
		// 	data->m_bubble_id = 10;
		// 	g_notification_service->push_success("Block Joins", std::format("Trying to prevent {} from joining..", player_name));
		// 	LOG(WARNING) << "Trying to prevent a blocked player " << player_name << " from joining.";
		// }

		bool result = g_hooking->get_original<hooks::send_non_physical_player_data>()(player, message, flags, a4, a5);

		// data->m_bubble_id = old_bubble_id;

		return result;
	}
}