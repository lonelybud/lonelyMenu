
#include "core/data/self.hpp"
#include "core/data/vehicle.hpp"
#include "gta/net_game_event.hpp"
#include "hooking/hooking.hpp"
#include "util/local_player.hpp"
#include "util/sync_trees.hpp"

#include <netsync/nodes/physical/CPhysicalScriptGameStateDataNode.hpp>
#include <netsync/nodes/player/CPlayerCameraDataNode.hpp>
#include <netsync/nodes/player/CPlayerGameStateDataNode.hpp>

namespace big
{
	void hooks::write_node_data(void* data_node, rage::netObject* net_object, rage::datBitBuffer* buffer, void* log, bool update)
	{
		bool node_updated                  = false;
		rage::datBitBuffer original_buffer = *buffer;
		g_hooking->get_original<hooks::write_node_data>()(data_node, net_object, buffer, log, update);

		const auto& node = sync_node_finder::find(reinterpret_cast<uint64_t>(data_node));

		switch (node)
		{
		case sync_node_id("CPhysicalScriptGameStateDataNode"):
		{
			auto node = reinterpret_cast<CPhysicalScriptGameStateDataNode*>(data_node);
			if (g_vehicle.god_mode && self::veh && g_local_player->m_vehicle && g_local_player->m_vehicle->m_net_object == net_object)
			{
				node->m_godmode         = false;
				node->m_bullet_proof    = false;
				node->m_fire_proof      = false;
				node->m_smoke_proof     = false;
				node->m_steam_proof     = false;
				node->m_collision_proof = false;
				node->m_explosion_proof = false;
				node->m_melee_proof     = false;
				node_updated            = true;
			}

			break;
		}
		case sync_node_id("CPlayerCameraDataNode"):
		{
			auto node = reinterpret_cast<CPlayerCameraDataNode*>(data_node);
			if (g_self.spectating)
			{
				node->m_free_cam_pos_x += 50.0f;
				node->m_free_cam_pos_y -= 50.0f;
				node->m_camera_x -= 50.0f;
				node_updated = true;
			}
			break;
		}
		case sync_node_id("CPlayerGameStateDataNode"):
		{
			auto node = reinterpret_cast<CPlayerGameStateDataNode*>(data_node);
			if (g_self.god_mode && !local_player::is_in_cutscene() && !local_player::is_in_interior())
			{
				node->m_is_invincible   = false;
				node->m_bullet_proof    = false;
				node->m_collision_proof = false;
				node->m_explosion_proof = false;
				node->m_fire_proof      = false;
				node->m_melee_proof     = false;
				node->m_steam_proof     = false;
				node->m_water_proof     = false;
				node_updated            = true;
			}

			if (g_self.spectating)
			{
				node->m_is_spectating     = false;
				node->m_spectating_net_id = 0;
				node_updated              = true;
			}

			break;
		}
		}

		if (node_updated)
		{
			*buffer = original_buffer;
			g_hooking->get_original<hooks::write_node_data>()(data_node, net_object, buffer, log, false);
		}
	}
}
