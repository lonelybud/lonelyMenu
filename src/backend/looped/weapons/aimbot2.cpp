#include "backend/looped_command.hpp"
#include "core/data/weapons.hpp"
#include "gta/enums.hpp"
#include "hooking/hooking.hpp"
#include "natives.hpp"
#include "util/entity.hpp"
#include "util/math.hpp"
#include "util/pools.hpp"

namespace big
{
	class aimbot2 : looped_command
	{
		using looped_command::looped_command;

	public:
		CPed* m_target{};
		bool is_aiming = false;

		inline uintptr_t get_cam_follow_ped_camera()
		{
			return *reinterpret_cast<uintptr_t*>(*g_pointers->m_gta.m_cam_gameplay_director + 0x3C0);
		}

		inline rage::fvector3 get_camera_position()
		{
			return *reinterpret_cast<rage::fvector3*>(get_cam_follow_ped_camera() + 0x60);
		}

		void find_best_target(CPed* self_ped, const rage::fvector3& self_pos)
		{
			m_target = nullptr;

			float best_distance            = g_weapons.aimbot.distance;
			float best_dist_to_mid_of_scrn = g_weapons.aimbot.max_dist_to_mid_of_scrn;

			for (rage::CEntity* ped_ : pools::get_all_peds())
			{
				CPed* ped = (CPed*)ped_;

				if (ped == self_ped)
					continue;

				if (ped->m_health <= 0)
					continue;

				if (g_weapons.aimbot.player)
				{
					if (!ped->m_player_info)
						continue;
				}
				else if (ped->m_player_info)
					continue;

				const auto ped_handle = g_pointers->m_gta.m_ptr_to_handle(ped);

				if (!ENTITY::HAS_ENTITY_CLEAR_LOS_TO_ENTITY(self::ped, ped_handle, eTraceFlags::IntersectWorld | eTraceFlags::IntersectObject))
					continue;

				const auto head_position = ped->get_bone_coords(ePedBoneType::HEAD);

				rage::fvector2 screen_pos;
				HUD::GET_HUD_SCREEN_POSITION_FROM_WORLD_POSITION(head_position.x,
				    head_position.y,
				    head_position.z,
				    &screen_pos.x,
				    &screen_pos.y);

				const auto distance_to_ped              = self_pos.distance(head_position);
				const auto distance_to_middle_of_screen = entity::distance_to_middle_of_screen(screen_pos);
				if (distance_to_middle_of_screen < best_dist_to_mid_of_scrn && distance_to_ped < best_distance)
				{
					best_distance            = distance_to_ped;
					best_dist_to_mid_of_scrn = distance_to_middle_of_screen;
					m_target                 = ped;
				}
			}
		}

		// Make aimbot works when driving a vehicle.
		void reset_aim_vectors(uintptr_t camera)
		{
			uintptr_t camera_params = *(uintptr_t*)(camera + 0x10);
			if (g_local_player->m_vehicle)
			{
				if (*(float*)(camera_params + 0x2AC) == -2.0f)
				{
					*(float*)(camera_params + 0x2AC) = 0.0f;
					*(float*)(camera_params + 0x2C0) = 111.0f;
					*(float*)(camera_params + 0x2C4) = 111.0f;
				}
			}
			else if (*(float*)(camera_params + 0x130) == 8.0f)
			{
				*(float*)(camera_params + 0x130) = 111.0f; // def 8.0f
				*(float*)(camera_params + 0x134) = 111.0f; // def 10.0f
				*(float*)(camera_params + 0x4CC) = 0.0f;   // def 4.0f

				if (*(float*)(camera_params + 0x49C) == 1.0f)
				{
					*(float*)(camera_params + 0x49C) = 0.0f; // def 1.0f
				}

				*(float*)(camera_params + 0x2AC) = 0.0f; // def -3.0f
				*(float*)(camera_params + 0x2B0) = 0.0f; // def -8.0f
			}
		}

		inline void compute_aim_direction_and_set_gameplay_cam(const rage::fvector3& target_bone_position)
		{
			uintptr_t cam_follow_ped_camera = get_cam_follow_ped_camera();

			const auto aim_direction = (target_bone_position - get_camera_position()).normalize();

			reset_aim_vectors(cam_follow_ped_camera);

			*reinterpret_cast<rage::fvector3*>(cam_follow_ped_camera + 0x40)  = aim_direction;
			*reinterpret_cast<rage::fvector3*>(cam_follow_ped_camera + 0x3D0) = aim_direction;
		}

		CPed* last_target_pos_target{};
		rage::fvector3 last_target_pos{};
		rage::fvector3 last_my_pos{};

		rage::fvector3 get_velocity(CPed* ped)
		{
			if (ped == g_local_player)
			{
				const auto velocity = *ped->get_position() - last_my_pos;
				last_my_pos         = *ped->get_position();
				return velocity;
			}
			else if (ped == last_target_pos_target)
			{
				const auto velocity = *ped->get_position() - last_target_pos;
				last_target_pos     = *ped->get_position();
				return velocity;
			}
			else
			{
				last_target_pos_target = ped;
				last_target_pos        = *ped->get_position();
				return {};
			}
		}

		void adjust_position_for_target_velocity(rage::fvector3& target_position)
		{
			const auto target_velocity = get_velocity(m_target);
			const auto my_velocity     = get_velocity(g_local_player);

			target_position += (target_velocity - my_velocity);
		}

		virtual void on_tick() override
		{
			is_aiming = PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_AIM);

			if (!is_aiming)
				return;

			CAM::STOP_SCRIPT_GLOBAL_SHAKING(1);
			CAM::SET_GAMEPLAY_CAM_SHAKE_AMPLITUDE(0);
		}
	};

	aimbot2 g_aimbot2("aimbot2", "Aimbot2", "", g_weapons.aimbot.enable_new);

	bool hooks::aimbot_cam_gameplay_director_update(uintptr_t this_)
	{
		const auto res = big::hooking::get_original<hooks::aimbot_cam_gameplay_director_update>()(this_);

		if (g_weapons.aimbot.enable_new && g_aimbot2.is_aiming)
		{
			g_aimbot2.find_best_target(g_local_player, *g_local_player->get_position());

			if (g_aimbot2.m_target)
			{
				auto target_bone_position = g_aimbot2.m_target->get_bone_coords(ePedBoneType::HEAD);
				// Take into account the target velocity.
				g_aimbot2.adjust_position_for_target_velocity(target_bone_position);
				g_aimbot2.compute_aim_direction_and_set_gameplay_cam(target_bone_position);
			}
		}

		return res;
	}
}
