#include "backend/looped_command.hpp"
#include "core/data/weapons.hpp"
#include "core/vars.hpp"
#include "gta/enums.hpp"
#include "gta/weapons.hpp"
#include "gui.hpp"
#include "natives.hpp"
#include "script.hpp"
// #include "util/math.hpp"
#include "util/timer.hpp"

namespace big
{
	class rapid_fire : looped_command
	{
		using looped_command::looped_command;

		CWeaponInfo* current_weapon = nullptr;

		Hash weapon_hash;
		int original_weapon_tint = 0;
		int current_weapon_tint  = 0;
		throttle<std::chrono::milliseconds> timer;
		int _delay = g_weapons.rapid_fire_delay;

		inline void reset_timer()
		{
			timer.reset(g_weapons.rapid_fire_delay);
			_delay = g_weapons.rapid_fire_delay;
		}

		virtual void on_enable() override
		{
			reset_timer();
		}

		virtual void on_tick() override
		{
			if (g_local_player == nullptr)
				return;

			PAD::DISABLE_CONTROL_ACTION(0, static_cast<int>(ControllerInputs::INPUT_ATTACK), TRUE);
			PAD::DISABLE_CONTROL_ACTION(0, static_cast<int>(ControllerInputs::INPUT_ATTACK2), TRUE);
			// when in vehicle
			PAD::DISABLE_CONTROL_ACTION(0, static_cast<int>(ControllerInputs::INPUT_VEH_ATTACK), TRUE);
			PAD::DISABLE_CONTROL_ACTION(0, static_cast<int>(ControllerInputs::INPUT_VEH_ATTACK2), TRUE);

			if (auto* const weapon_mgr = g_local_player->m_weapon_manager)
			{
				if (!current_weapon)
				{
					current_weapon = weapon_mgr->m_weapon_info;

					WEAPON::GET_CURRENT_PED_WEAPON(self::ped, &weapon_hash, FALSE);
					if (weapon_hash == "WEAPON_FLAREGUN"_J)
						current_weapon_tint = original_weapon_tint = WEAPON::GET_PED_WEAPON_TINT_INDEX(self::ped, weapon_hash);
				}

				if (current_weapon != weapon_mgr->m_weapon_info || weapon_mgr->m_weapon_info->m_group == GROUP_MELEE)
				{
					g_weapons.rapid_fire = false;
					on_disable();
				}
				else if (!g_gui->is_open() && PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_ATTACK) && PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_AIM))
				{
					if (_delay != g_weapons.rapid_fire_delay)
						reset_timer();
					else if (timer.has_time_passed())
					{
						// auto camera_direction = math::rotation_to_direction(CAM::GET_GAMEPLAY_CAM_ROT(0));
						// auto camera_position  = CAM::GET_GAMEPLAY_CAM_COORD() + camera_direction;
						// Vector3 end           = camera_position + camera_direction * 2000.0;

						if (weapon_hash == "WEAPON_FLAREGUN"_J)
						{
							WEAPON::SET_PED_WEAPON_TINT_INDEX(self::ped, weapon_hash, current_weapon_tint++);
							if (current_weapon_tint > 7)
								current_weapon_tint = 0;
						}

						// PED::SET_PED_SHOOTS_AT_COORD(self::ped, end.x, end.y, end.z, 0);

						// https://github.com/Illegal-Services/v_ped_stuff/blob/main/ePedResetFlags.json
						PED::SET_PED_RESET_FLAG(self::ped, 65, TRUE);
						WEAPON::REFILL_AMMO_INSTANTLY(self::ped);
					}
				}
			}
		}

		virtual void on_disable() override
		{
			if (current_weapon && weapon_hash == "WEAPON_FLAREGUN"_J)
				WEAPON::SET_PED_WEAPON_TINT_INDEX(self::ped, weapon_hash, original_weapon_tint);

			current_weapon = nullptr;
		}
	};

	rapid_fire g_rapid_fire("rapidfire", "Rapid Fire", "Makes your weapon fire insanely fast", g_weapons.rapid_fire);
}
