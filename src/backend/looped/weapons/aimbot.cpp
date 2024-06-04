#include "backend/looped_command.hpp"
#include "core/data/weapons.hpp"
#include "core/vars.hpp"
#include "gta/enums.hpp"
#include "natives.hpp"
#include "services/players/player_service.hpp"
#include "util/entity.hpp"
#include "util/pools.hpp"

#include <imgui.h>
#include <numbers>

namespace big
{
	class aimbot : looped_command
	{
		using looped_command::looped_command;

		CPed* last_target = nullptr;
		rage::fvector3 last_target_pos{};
		rage::fvector3 last_my_pos{};
		bool velocity_reset = true;

		rage::fvector3 get_velocity_diff(CPed* target)
		{
			const auto local_player_curr_pos = *g_local_player->get_position();
			const auto target_curr_pos       = *target->get_position();

			if (velocity_reset)
			{
				last_my_pos     = local_player_curr_pos;
				last_target_pos = target_curr_pos;
				velocity_reset  = false;
				return {};
			}

			const auto local_player_velocity = local_player_curr_pos - last_my_pos;
			last_my_pos                      = local_player_curr_pos;

			const auto target_velocity = target_curr_pos - last_target_pos;
			last_target_pos            = target_curr_pos;

			return target_velocity - local_player_velocity;
		}

		void cped_callback(float& distance, CPed* ped, CPed** m_target_ref, rage::fvector3& bone_pos)
		{
			if (!ped || ped->m_health <= 0)
				return;

			auto _bone_pos        = ped->get_bone_coords(g_weapons.aimbot.bone);
			const auto ped_handle = g_pointers->m_gta.m_ptr_to_handle(ped);

			if (!ENTITY::HAS_ENTITY_CLEAR_LOS_TO_ENTITY(self::ped, ped_handle, eTraceFlags::IntersectWorld | eTraceFlags::IntersectObject))
				return;

			rage::fvector2 screen_pos;
			HUD::GET_HUD_SCREEN_POSITION_FROM_WORLD_POSITION(_bone_pos.x,
			    _bone_pos.y,
			    _bone_pos.z,
			    &screen_pos.x,
			    &screen_pos.y);
			if (auto dist = entity::distance_to_middle_of_screen(screen_pos); dist <= distance)
			{
				*m_target_ref = ped;
				distance      = dist;
				bone_pos      = _bone_pos;
			}
		}

		virtual void on_tick() override
		{
			if (PAD::GET_DISABLED_CONTROL_NORMAL(0, (int)ControllerInputs::INPUT_AIM))
			{
				CPed* m_target = nullptr;
				float distance = g_weapons.aimbot.max_dist_to_mid_of_scrn;
				rage::fvector3 bone_pos;

				if (g_weapons.aimbot.player)
				{
					for (auto& p : g_player_service->players())
						if (p.second && p.second->is_valid())
							cped_callback(distance, p.second->get_ped(), &m_target, bone_pos);
				}
				else
				{
					for (rage::CEntity* ped_ : pools::get_all_peds())
					{
						CPed* ped = (CPed*)ped_;

						if (!ped || ped->m_player_info)
							continue;

						cped_callback(distance, ped, &m_target, bone_pos);
					}
				}

				if (m_target)
				{
					if (last_target != m_target)
					{
						last_target    = m_target;
						velocity_reset = true;
					}

					bone_pos += get_velocity_diff(m_target);

					auto final_rend_cam_coord = CAM::GET_FINAL_RENDERED_CAM_COORD();
					Vector3 _bone_pos         = bone_pos;
					Vector3 camera_target     = _bone_pos - final_rend_cam_coord;
					float RADPI               = 180.0f / std::numbers::pi;
					float camera_heading      = atan2f(camera_target.x, camera_target.y) * RADPI;
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
			else if (last_target)
				last_target = nullptr;
		}
	};

	aimbot g_aimbot("aimbot", "Aimbot", "Lock on and kill", g_weapons.aimbot.enable);
}
