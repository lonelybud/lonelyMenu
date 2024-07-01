#include "backend/looped_command.hpp"
#include "core/data/weapons.hpp"
#include "core/vars.hpp"

namespace big
{
	class ammo_special_type : looped_command
	{
		using looped_command::looped_command;

		CWeaponInfo* p_modified_weapon = nullptr;
		CWeaponInfo::sExplosion modified_weapon_explosion{};
		eDamageType modified_weapon_damage_type = eDamageType::None;
		eExplosionTag explosion_tag             = eExplosionTag::DONTCARE;

		void reset_to_og()
		{
			if (p_modified_weapon)
			{
				p_modified_weapon->m_damage_type = modified_weapon_damage_type;
				p_modified_weapon->m_explosion   = modified_weapon_explosion;
			}
			explosion_tag = g_weapons.explosion_tag;
		}

		virtual void on_tick() override
		{
			if (auto* const weapon_mgr = g_local_player->m_weapon_manager;
			    weapon_mgr && (p_modified_weapon != weapon_mgr->m_weapon_info || explosion_tag != g_weapons.explosion_tag))
			{
				reset_to_og();

				if (p_modified_weapon = weapon_mgr->m_weapon_info)
				{
					// Backup
					{
						modified_weapon_damage_type = p_modified_weapon->m_damage_type;
						modified_weapon_explosion   = p_modified_weapon->m_explosion;
					}

					// Set to the good stuff
					{
						eExplosionTag explosion_tag = g_weapons.explosion_tag;

						if (explosion_tag != eExplosionTag::DONTCARE)
							p_modified_weapon->m_damage_type = eDamageType::Explosive;

						CWeaponInfo::sExplosion explosion;
						explosion.m_default            = explosion_tag;
						explosion.m_hit_bike           = explosion_tag;
						explosion.m_hit_boat           = explosion_tag;
						explosion.m_hit_car            = explosion_tag;
						explosion.m_hit_plane          = explosion_tag;
						explosion.m_hit_truck          = explosion_tag;
						p_modified_weapon->m_explosion = explosion;
					}
				}
			}
		}

		virtual void on_disable() override
		{
			reset_to_og();
			p_modified_weapon = nullptr;
		}
	};

	ammo_special_type g_ammo_special_type("ammo_special_type", "Enable Ammo Special", "", g_weapons.ammo_special_type);
}
