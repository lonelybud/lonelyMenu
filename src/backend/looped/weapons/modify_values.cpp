#include "backend/looped/looped.hpp"
#include "backend/looped_command.hpp"
#include "core/data/weapons.hpp"
#include "core/vars.hpp"

namespace big
{
	class modify_weapon_values : looped_command
	{
		using looped_command::looped_command;

		CWeaponInfo* p_modified_weapon = nullptr;
		float m_time_between_shots     = 0.0f;

		inline void reset()
		{
			if (p_modified_weapon)
			{
				p_modified_weapon->m_time_between_shots = m_time_between_shots;
				p_modified_weapon                       = nullptr;
			}
		}

		virtual void on_tick() override
		{
			if (g_local_player == nullptr)
				return;

			if (auto* const weapon_mgr = g_local_player->m_weapon_manager)
			{
				if (p_modified_weapon != weapon_mgr->m_weapon_info)
				{
					reset();

					if (weapon_mgr->m_weapon_info)
					{
						p_modified_weapon = weapon_mgr->m_weapon_info;
						g_weapons.modify_weapon_values.time_bw_shots = m_time_between_shots = p_modified_weapon->m_time_between_shots;
					}
				}
				else if (p_modified_weapon)
				{
					p_modified_weapon->m_time_between_shots = g_weapons.modify_weapon_values.time_bw_shots;
				}
			}
		}

		virtual void on_disable() override
		{
			reset();
		}
	};


	modify_weapon_values g_modify_weapon_values("modify_weapon_values", "Modify weapon Values", "enable it to apply various values.",
	    g_weapons.modify_weapon_values.enable);
}