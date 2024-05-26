#include "core/data/desync_kick.hpp"
#include "pointers.hpp"
#include "services/players/player_service.hpp"

#include <imgui.h>

namespace big
{
	inline void view_debug_info()
	{
		if (ImGui::BeginTabItem("Info"))
		{
			ImGui::Text("Game Version: %s", g_pointers->m_gta.m_game_version);
			ImGui::Text("Online Version: %s", g_pointers->m_gta.m_online_version);
			ImGui::Text("Session Started?: %d", *g_pointers->m_gta.m_is_session_started);
			ImGui::Text("No of players: %d", g_player_service->players().size() + 1);
			ImGui::Text("No of Deyncs Active: %d", g_desync_kick_players.size());

			ImGui::EndTabItem();
		}
	}
}
