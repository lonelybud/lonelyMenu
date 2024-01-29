#include "core/data/self.hpp"
#include "hooking/hooking.hpp"

#include <netsync/nodes/player/CPlayerCameraDataNode.hpp>
#include <netsync/nodes/player/CPlayerGameStateDataNode.hpp>

namespace big
{
	void hooks::write_player_camera_data_node(rage::netObject* player, CPlayerCameraDataNode* node)
	{
		g_hooking->get_original<hooks::write_player_camera_data_node>()(player, node);

		if (g_self.spectating)
		{
			node->m_free_cam_pos_x += 50.0f;
			node->m_free_cam_pos_y -= 50.0f;
			node->m_camera_x -= 50.0f;
		}
	}
}
