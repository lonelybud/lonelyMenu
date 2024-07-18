#include "backend/looped_command.hpp"
#include "core/data/weapons.hpp"
#include "core/vars.hpp"
#include "gta/enums.hpp"
#include "hooking/hooking.hpp"
#include "natives.hpp"
#include "services/players/player_service.hpp"
#include "util/entity.hpp"
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

		inline void cped_callback(float& distance, CPed* ped)
		{
			if (!ped || ped->m_health <= 0)
				return;

			auto their_bone_pos   = ped->get_bone_coords(g_weapons.aimbot.bone);
			const auto ped_handle = g_pointers->m_gta.m_ptr_to_handle(ped);

			if (g_local_player)
			{
				const auto my_head_pos = g_local_player->get_bone_coords(ePedBoneType::HEAD);
				constexpr auto los_flags = (ST_OPTION_IGNORE_GLASS | ST_OPTION_IGNORE_NOTHING | ST_OPTION_IGNORE_TRANSPARENT);
				auto shape_test_handle = SHAPETEST::START_EXPENSIVE_SYNCHRONOUS_SHAPE_TEST_LOS_PROBE(my_head_pos.x,
				    my_head_pos.y,
				    my_head_pos.z,
				    their_bone_pos.x,
				    their_bone_pos.y,
				    their_bone_pos.z,
				    ST_INCLUDE_ALL,
				    self::ped,
				    los_flags);
				BOOL did_shapetest_hit{};
				Vector3 dont_care;
				Entity entity_hit{};
				if (SHAPETEST::GET_SHAPE_TEST_RESULT(shape_test_handle, &did_shapetest_hit, &dont_care, &dont_care, &entity_hit) && did_shapetest_hit && entity_hit != ped_handle)
					return;
			}

			rage::fvector2 screen_pos;
			HUD::GET_HUD_SCREEN_POSITION_FROM_WORLD_POSITION(their_bone_pos.x,
			    their_bone_pos.y,
			    their_bone_pos.z,
			    &screen_pos.x,
			    &screen_pos.y);

			if (auto dist = entity::distance_to_middle_of_screen(screen_pos); dist <= distance)
			{
				m_target = ped;
				distance = dist;
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

		void compute_aim_direction_and_set_gameplay_cam(const rage::fvector3& target_bone_position)
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

		inline void adjust_position_for_target_velocity(rage::fvector3& target_position)
		{
			const auto target_velocity = get_velocity(m_target);
			const auto my_velocity     = get_velocity(g_local_player);

			if (target_velocity == rage::fvector3{})
				return;

			target_position += (target_velocity - my_velocity);
		}

		virtual void on_tick() override
		{
			if (is_aiming = PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_AIM))
			{
				CAM::STOP_SCRIPT_GLOBAL_SHAKING(TRUE);
				CAM::SET_GAMEPLAY_CAM_SHAKE_AMPLITUDE(0);
			}
		}
	};

	aimbot2 g_aimbot2("aimbot2", "Aimbot2", "Original Yim Aimbot", g_weapons.aimbot.enable_2);

	bool hooks::aimbot_cam_gameplay_director_update(uintptr_t this_)
	{
		const auto res = big::hooking::get_original<hooks::aimbot_cam_gameplay_director_update>()(this_);

		if (g_weapons.aimbot.enable_2 && g_aimbot2.is_aiming)
		{
			g_aimbot2.m_target             = nullptr;
			float best_dist_to_mid_of_scrn = g_weapons.aimbot.max_dist_to_mid_of_scrn;

			if (g_weapons.aimbot.player)
			{
				for (auto& p : g_player_service->players())
					if (p.second && p.second->is_valid())
						g_aimbot2.cped_callback(best_dist_to_mid_of_scrn, p.second->get_ped());
			}
			else
			{
				for (rage::CEntity* ped_ : pools::get_all_peds())
				{
					CPed* ped = (CPed*)ped_;

					if (!ped || ped->m_player_info)
						continue;

					g_aimbot2.cped_callback(best_dist_to_mid_of_scrn, ped);
				}
			}

			if (g_aimbot2.m_target)
			{
				auto target_bone_position = g_aimbot2.m_target->get_bone_coords(g_weapons.aimbot.bone);
				// Take into account the target velocity.
				if (g_weapons.aimbot.check_velocity)
					g_aimbot2.adjust_position_for_target_velocity(target_bone_position);
				g_aimbot2.compute_aim_direction_and_set_gameplay_cam(target_bone_position);
			}
		}

		return res;
	}
}