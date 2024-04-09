#include "backend/looped_command.hpp"
#include "core/data/weapons.hpp"
#include "gta/enums.hpp"
#include "natives.hpp"
#include "util/entity.hpp"

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
				Entity ped{};
				float distance = g_weapons.aimbot.max_dist_to_mid_of_scrn;
				Vector3 aim_lock;

				for (auto ent : entity::get_entities(false, true))
				{
					if (ENTITY::IS_ENTITY_DEAD(ent, 0))
						continue;

					auto is_player = PED::IS_PED_A_PLAYER(ent);

					if (g_weapons.aimbot.player)
					{
						if (!is_player)
							continue;
					}
					else if (is_player)
						continue;

					Vector3 pos      = ENTITY::GET_ENTITY_COORDS(ent, 0);
					Vector3 bone_pos = PED::GET_PED_BONE_COORDS(ent, (int)PedBones::SKEL_Head, 0, 0, 0);

					float distance_between_target = SYSTEM::VDIST2(self::pos.x, self::pos.y, self::pos.z, pos.x, pos.y, pos.z);

					if (distance_between_target > (g_weapons.aimbot.distance * g_weapons.aimbot.distance))
						continue;

					if (!ENTITY::HAS_ENTITY_CLEAR_LOS_TO_ENTITY_ADJUST_FOR_COVER(self::ped, ent, 17))
						continue;

					rage::fvector2 screen_pos;
					HUD::GET_HUD_SCREEN_POSITION_FROM_WORLD_POSITION(bone_pos.x,
					    bone_pos.y,
					    bone_pos.z,
					    &screen_pos.x,
					    &screen_pos.y);

					if (entity::distance_to_middle_of_screen(screen_pos) < distance && ENTITY::IS_ENTITY_ON_SCREEN(ent) && ent != self::ped)
					{
						ped      = ent;
						distance = entity::distance_to_middle_of_screen(screen_pos);
						aim_lock = bone_pos;
					}
				}

				if (ped)
				{
					GRAPHICS::DRAW_MARKER(3, aim_lock.x, aim_lock.y, aim_lock.z + 0.5f, 0, 0, 0, 0, 180, 0, 0.3f, 0.3f, 0.3f, 255, 255, 255, 255, 1, 1, 0, 0, 0, 0, 0);

					Vector3 camera_target = aim_lock - CAM::GET_FINAL_RENDERED_CAM_COORD();
					float RADPI           = 180.0f / std::numbers::pi;

					float camera_heading = atan2f(camera_target.x, camera_target.y) * RADPI;
					if (camera_heading >= 0.0f && camera_heading <= 180.0f)
						camera_heading = 360.0f - camera_heading;
					else if (camera_heading <= -0.0f && camera_heading >= -180.0f)
						camera_heading = -camera_heading;
					float self_heading = ENTITY::GET_ENTITY_HEADING(self::veh ? self::veh : self::ped);
					CAM::SET_GAMEPLAY_CAM_RELATIVE_HEADING(camera_heading - self_heading);

					float magnitude    = std::hypot(camera_target.x, camera_target.y, camera_target.z);
					float camera_pitch = asinf(camera_target.z / magnitude) * RADPI;
					float self_pitch   = ENTITY::GET_ENTITY_PITCH(self::veh ? self::veh : self::ped);
					CAM::SET_GAMEPLAY_CAM_RELATIVE_PITCH(camera_pitch - self_pitch, 1.0f);
				}
			}
		}
	};

	aimbot g_aimbot("aimbot", "Aimbot", "", g_weapons.aimbot.enable);
}
