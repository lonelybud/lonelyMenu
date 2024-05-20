#include "backend/looped_command.hpp"
#include "core/data/weapons.hpp"
#include "gta/enums.hpp"
#include "natives.hpp"
#include "util/entity.hpp"
#include "util/pools.hpp"

#include <imgui.h>
#include <numbers>

namespace big
{
	class aimbot : looped_command
	{
		using looped_command::looped_command;

		virtual void on_tick() override
		{
			if (PAD::GET_DISABLED_CONTROL_NORMAL(0, (int)ControllerInputs::INPUT_AIM))
			{
				CPed* m_target = nullptr;
				float distance = g_weapons.aimbot.max_dist_to_mid_of_scrn;

				for (rage::CEntity* ped_ : pools::get_all_peds())
				{
					CPed* ped = (CPed*)ped_;

					if (ped == g_local_player || ped->m_health <= 0)
						continue;

					auto is_player = ped->m_player_info;

					if (g_weapons.aimbot.player)
					{
						if (!is_player)
							continue;
					}
					else if (is_player)
						continue;

					auto bone_pos         = ped->get_bone_coords(g_weapons.aimbot.bone);
					const auto ped_handle = g_pointers->m_gta.m_ptr_to_handle(ped);

					if (!ENTITY::HAS_ENTITY_CLEAR_LOS_TO_ENTITY(self::ped, ped_handle, eTraceFlags::IntersectWorld | eTraceFlags::IntersectObject))
						continue;

					rage::fvector2 screen_pos;
					HUD::GET_HUD_SCREEN_POSITION_FROM_WORLD_POSITION(bone_pos.x,
					    bone_pos.y,
					    bone_pos.z,
					    &screen_pos.x,
					    &screen_pos.y);
					if (auto dist = entity::distance_to_middle_of_screen(screen_pos); dist <= distance)
					{
						m_target = ped;
						distance = dist;
					}
				}

				if (m_target)
				{
					auto bone_pos             = m_target->get_bone_coords(g_weapons.aimbot.bone);
					auto final_rend_cam_coord = CAM::GET_FINAL_RENDERED_CAM_COORD();
					Vector3 camera_target     = {bone_pos.x - final_rend_cam_coord.x,
					        bone_pos.y - final_rend_cam_coord.y,
					        bone_pos.z - final_rend_cam_coord.z};
					float RADPI               = 180.0f / std::numbers::pi;

					float camera_heading = atan2f(camera_target.x, camera_target.y) * RADPI;
					if (camera_heading >= 0.0f && camera_heading <= 180.0f)
						camera_heading = 360.0f - camera_heading;
					else if (camera_heading <= -0.0f && camera_heading >= -180.0f)
						camera_heading = -camera_heading;
					CAM::SET_GAMEPLAY_CAM_RELATIVE_HEADING(camera_heading - ENTITY::GET_ENTITY_HEADING(self::ped));

					float magnitude    = std::hypot(camera_target.x, camera_target.y, camera_target.z);
					float camera_pitch = asinf(camera_target.z / magnitude) * RADPI;
					CAM::SET_GAMEPLAY_CAM_RELATIVE_PITCH(camera_pitch - ENTITY::GET_ENTITY_PITCH(self::ped), 1.0f);
				}
			}
		}
	};

	aimbot g_aimbot("aimbot", "Aimbot", "Lock on and kill", g_weapons.aimbot.enable);
}
