#include "backend/looped_command.hpp"
#include "core/data/weapons.hpp"
#include "core/vars.hpp"

namespace big
{
	class no_recoil_spread : looped_command
	{
		using looped_command::looped_command;

		CWeaponInfo* p_modified_weapon = nullptr;

		float og_bullet_speed           = 0;
		float og_recoil_shake_amplitude = 0;
		uint32_t og_recoil_hash         = 0;
		uint32_t og_recoil_hash_fp      = 0;

		float og_accuracy_spread         = 0;
		uint32_t og_accuracy_offset_hash = 0;

		void reset_to_og()
		{
			p_modified_weapon->m_speed                          = og_bullet_speed;
			p_modified_weapon->m_recoil_shake_amplitude         = og_recoil_shake_amplitude;
			p_modified_weapon->m_recoil_shake_hash              = og_recoil_hash;
			p_modified_weapon->m_recoil_shake_hash_first_person = og_recoil_hash_fp;

			p_modified_weapon->m_accuracy_spread            = og_accuracy_spread;
			p_modified_weapon->m_accuracy_offset_shake_hash = og_accuracy_offset_hash;
		}

		virtual void on_tick() override
		{
			if (auto* const weapon_mgr = g_local_player->m_weapon_manager; weapon_mgr && p_modified_weapon != weapon_mgr->m_weapon_info)
			{
				if (p_modified_weapon)
					reset_to_og();

				if (p_modified_weapon = weapon_mgr->m_weapon_info)
				{
					// Backup
					{
						og_bullet_speed           = p_modified_weapon->m_speed;
						og_recoil_shake_amplitude = p_modified_weapon->m_recoil_shake_amplitude;
						og_recoil_hash            = p_modified_weapon->m_recoil_shake_hash;
						og_recoil_hash_fp         = p_modified_weapon->m_recoil_shake_hash_first_person;

						og_accuracy_spread      = p_modified_weapon->m_accuracy_spread;
						og_accuracy_offset_hash = p_modified_weapon->m_accuracy_offset_shake_hash;
					}

					// Set to the good stuff
					{
						p_modified_weapon->m_speed                          = 9999999999.0f;
						p_modified_weapon->m_recoil_shake_amplitude         = 0;
						p_modified_weapon->m_recoil_shake_hash              = 0;
						p_modified_weapon->m_recoil_shake_hash_first_person = 0;

						p_modified_weapon->m_accuracy_spread            = 0;
						p_modified_weapon->m_accuracy_offset_shake_hash = 0;
					}
				}
			}
		}

		virtual void on_disable() override
		{
			if (p_modified_weapon)
			{
				reset_to_og();
				p_modified_weapon = nullptr;
			}
		}
	};

	no_recoil_spread g_no_recoil_spread("norecoilspread", "No Recoil & Spread", "", g_weapons.no_recoil_spread);
}