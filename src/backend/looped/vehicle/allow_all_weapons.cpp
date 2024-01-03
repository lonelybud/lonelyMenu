#include "backend/looped_command.hpp"
#include "core/data/vehicle.hpp"
#include "gta/enums.hpp"
#include "gta/weapons.hpp"
#include "pointers.hpp"

class CVehicleDriveByAnimInfo;

#include <vehicle/CGetPedSeatReturnClass.hpp>
#include <vehicle/CVehicleDriveByAnimInfo.hpp>
#include <vehicle/CVehicleDriveByMetadataMgr.hpp>
#include <vehicle/CVehicleModelInfo.hpp>
#include <vehicle/CVehicleSeatMetadataMgr.hpp>

namespace big
{
	class allow_all_weapons : looped_command
	{
		using looped_command::looped_command;

		CGetPedSeatReturnClass* seat_info = nullptr;

		inline CGetPedSeatReturnClass* get_seat_info()
		{
			return g_pointers->m_gta.m_get_ped_seat(g_local_player->m_seat_info, g_local_player);
		}

		void restore()
		{
			g_vehicle.unlimited_weapons = false;

			if (seat_info && seat_info->anim_info)
				for (auto drive_by_anim_info : seat_info->anim_info->m_drive_by_anim_infos)
					if (drive_by_anim_info->m_weapon_groups->m_groups.size() == 7
					    && drive_by_anim_info->m_weapon_groups->m_groups.contains(GROUP_PISTOL))
					{
						drive_by_anim_info->m_weapon_groups->m_groups.clear();
						drive_by_anim_info->m_weapon_groups->m_groups.append({GROUP_PISTOL});
					}

			seat_info = nullptr;
		}

		virtual void on_tick() override
		{
			if (g_local_player)
			{
				if (auto vehicle = g_local_player->m_vehicle)
				{
					if (const auto is_not_in_vehicle = !PED::GET_PED_CONFIG_FLAG(self::ped, 62, false))
					{
						restore();
						return;
					}

					if (!seat_info && (seat_info = get_seat_info()))
					{
						if (!seat_info->anim_info)
							return;

						for (auto drive_by_anim_info : seat_info->anim_info->m_drive_by_anim_infos)
							if (drive_by_anim_info->m_weapon_groups->m_groups.size() != 7
							    && drive_by_anim_info->m_weapon_groups->m_groups.contains(GROUP_PISTOL))
							{
								drive_by_anim_info->m_weapon_groups->m_groups.clear();
								drive_by_anim_info->m_weapon_groups->m_groups.append({GROUP_PISTOL, GROUP_MG, GROUP_RIFLE, GROUP_SHOTGUN, GROUP_HEAVY, GROUP_SNIPER, GROUP_SMG});
							}
					}
					else if (auto vehicle_model_info = static_cast<CVehicleModelInfo*>(g_local_player->m_vehicle->m_model_info))
					{
						vehicle_model_info->set_vehicle_model_flag(CVehicleModelInfoFlags::DRIVER_NO_DRIVE_BY, false);
						if (PAD::IS_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_AIM))
							PAD::DISABLE_CONTROL_ACTION(0, (int)ControllerInputs::INPUT_VEH_FLY_MOUSE_CONTROL_OVERRIDE, 1);
					}
				}
			}
		}

		virtual void on_disable() override
		{
			restore();
		}
	};

	allow_all_weapons g_allow_all_weapons("allow_all_weapons", "Allow Weapons In Vehicle", "", g_vehicle.unlimited_weapons);
}