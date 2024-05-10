#include "backend/looped_command.hpp"
#include "core/data/weapons.hpp"
#include "gta/enums.hpp"
#include "gui.hpp"
#include "natives.hpp"
#include "script.hpp"
#include "util/debouncer.hpp"
#include "util/math.hpp"

namespace big
{
	class rapid_fire : looped_command
	{
		using looped_command::looped_command;

		CWeaponInfo* current_weapon = nullptr;

		Hash weapon_hash;
		int original_weapon_tint = 0;
		int current_weapon_tint  = 0;
		debouncer<std::chrono::milliseconds> deb;
		int _delay = g_weapons.rapid_fire_delay;

		inline void reset_debouncer()
		{
			deb.reset(g_weapons.rapid_fire_delay);
			_delay = g_weapons.rapid_fire_delay;
		}

		virtual void on_enable() override
		{
			reset_debouncer();
		}

		virtual void on_tick() override
		{
			if (g_local_player == nullptr)
				return;

			if (auto* const weapon_mgr = g_local_player->m_weapon_manager)
			{
				if (!current_weapon)
				{
					current_weapon = weapon_mgr->m_weapon_info;

					WEAPON::GET_CURRENT_PED_WEAPON(self::ped, &weapon_hash, FALSE);
					if (weapon_hash == "WEAPON_FLAREGUN"_J)
						current_weapon_tint = original_weapon_tint = WEAPON::GET_PED_WEAPON_TINT_INDEX(self::ped, weapon_hash);
				}

				if (current_weapon != weapon_mgr->m_weapon_info)
				{
					g_weapons.rapid_fire = false;
					on_disable();
				}
				else if (!g_gui->is_open() && PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_ATTACK) && PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_AIM))
				{
					auto camera_direction = math::rotation_to_direction(CAM::GET_GAMEPLAY_CAM_ROT(0));
					auto camera_position  = CAM::GET_GAMEPLAY_CAM_COORD() + camera_direction;
					Vector3 end           = camera_position + camera_direction * 2000.0;

					if (weapon_hash == "WEAPON_FLAREGUN"_J)
					{
						WEAPON::SET_PED_WEAPON_TINT_INDEX(self::ped, weapon_hash, current_weapon_tint++);
						if (current_weapon_tint > 7)
							current_weapon_tint = 0;
					}

					if (_delay != g_weapons.rapid_fire_delay)
						reset_debouncer();
					else if (deb.has_debounced())
					{
						PED::SET_PED_SHOOTS_AT_COORD(self::ped, end.x, end.y, end.z, 0);
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
