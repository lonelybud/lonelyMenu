#include "hooking/hooking.hpp"
#include "services/players/player_service.hpp"
#include "util/globals.hpp"

#include <netsync/nodes/player/CPlayerGameStateDataNode.hpp>

namespace big
{
	static bool is_in_cutscene()
	{
		if (g_local_player && g_local_player->m_player_info)
			return g_local_player->m_player_info->m_game_state == eGameState::InMPCutscene;
		return false;
	}

	static bool is_in_interior()
	{
		int id = 0;
		if (auto self_ptr = g_player_service->get_self(); self_ptr->is_valid())
			id = self_ptr->id();
		return globals::get_interior_from_player(id) != 0;
	}

	void hooks::write_player_game_state_data_node(rage::netObject* player, CPlayerGameStateDataNode* node)
	{
		g_hooking->get_original<write_player_game_state_data_node>()(player, node);

		if (!is_in_cutscene() && !is_in_interior())
			node->m_is_invincible = false;
	}
}
