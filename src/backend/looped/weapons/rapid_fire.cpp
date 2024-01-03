#include "backend/looped_command.hpp"
#include "core/data/weapons.hpp"
#include "gta/enums.hpp"
#include "gui.hpp"
#include "natives.hpp"
#include "util/math.hpp"

namespace big
{
	class rapid_fire : looped_command
	{
		using looped_command::looped_command;

		CWeaponInfo* current_weapon = nullptr;
		Hash weapon_hash;
		Entity weapon_entity;
		Vector3 dimensions;

		virtual void on_tick() override
		{
			if (auto* const weapon_mgr = g_local_player->m_weapon_manager; weapon_mgr)
			{
				if (!current_weapon)
				{
					current_weapon = weapon_mgr->m_weapon_info;

					WEAPON::GET_CURRENT_PED_WEAPON(self::ped, &weapon_hash, false);

					if (weapon_hash != RAGE_JOAAT("WEAPON_UNARMED"))
					{
						weapon_entity = WEAPON::GET_CURRENT_PED_WEAPON_ENTITY_INDEX(self::ped, 0);
						Vector3 dim_min, dim_max;
						MISC::GET_MODEL_DIMENSIONS(ENTITY::GET_ENTITY_MODEL(weapon_entity), &dim_min, &dim_max);
						dimensions = (dim_max - dim_min) * 0.5f;
					}
				}

				if (current_weapon != weapon_mgr->m_weapon_info)
				{
					g_weapons.rapid_fire = false;
					on_disable();
				}
				else if (weapon_entity && !g_gui->is_open() && PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_ATTACK))
				{
					const auto weapon_position =
					    ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(weapon_entity, dimensions.x, 0, dimensions.z * 0.3f);

					auto camera_direction = math::rotation_to_direction(CAM::GET_GAMEPLAY_CAM_ROT(0));
					auto camera_position  = CAM::GET_GAMEPLAY_CAM_COORD() + camera_direction;

					Vector3 end = camera_position + camera_direction * 2000.0;

					const auto raycast_handle =
					    SHAPETEST::START_EXPENSIVE_SYNCHRONOUS_SHAPE_TEST_LOS_PROBE(camera_position.x,
					        camera_position.y,
					        camera_position.z,
					        end.x,
					        end.y,
					        end.z,
					        -1,
					        0,
					        7);
					int did_raycast_hit = 0;
					Vector3 raycast_hit_position{};
					Vector3 raycast_surface_normal_hit_position{};
					Entity raycast_hit_entity{};
					SHAPETEST::GET_SHAPE_TEST_RESULT(raycast_handle, &did_raycast_hit, &raycast_hit_position, &raycast_surface_normal_hit_position, &raycast_hit_entity);

					if (did_raycast_hit)
						end = raycast_hit_position;

					MISC::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(weapon_position.x, weapon_position.y, weapon_position.z, end.x, end.y, end.z, WEAPON::GET_WEAPON_DAMAGE(weapon_hash, 0), true, weapon_hash, self::ped, true, false, -1.0);
				}
			}
		}

		virtual void on_disable() override
		{
			current_weapon = nullptr;
			weapon_entity  = 0;
		}
	};

	rapid_fire g_rapid_fire("rapidfire", "Rapid Fire", "Makes your weapon fire insanely fast", g_weapons.rapid_fire);
}
