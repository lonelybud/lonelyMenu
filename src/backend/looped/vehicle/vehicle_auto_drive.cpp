#include "backend/looped/looped.hpp"
#include "core/data/auto_drive.hpp"
#include "core/enums.hpp"
#include "natives.hpp"
#include "util/blip.hpp"
#include "core/vars.hpp"

namespace big
{
	static std::unordered_map<AutoDriveStyle, int> driving_style_flags = {{AutoDriveStyle::LAW_ABIDING, 443}, {AutoDriveStyle::THE_ROAD_IS_YOURS, 787004}};

	void looped::vehicle_auto_drive()
	{
		static int current_driving_flag = driving_style_flags[AutoDriveStyle::LAW_ABIDING];
		static float current_speed      = 8;

		static bool has_driving_settings_changed = false;

		// start driving if destination is there
		if (g_auto_drive.auto_drive_destination != AutoDriveDestination::STOPPED)
		{
			if (!self::veh)
			{
				g_auto_drive.auto_drive_destination = AutoDriveDestination::STOPPED;
				has_driving_settings_changed        = false;
				g_auto_drive.is_auto_driving        = false;
			}

			// check for changing driving settings
			if (current_driving_flag != driving_style_flags[g_auto_drive.auto_drive_style]
			    || current_speed != g_auto_drive.auto_drive_speed)
			{
				current_driving_flag         = driving_style_flags[g_auto_drive.auto_drive_style];
				current_speed                = g_auto_drive.auto_drive_speed;
				has_driving_settings_changed = true;
			}

			if (!g_auto_drive.is_auto_driving)
			{
				auto location = g_auto_drive.auto_drive_destination == AutoDriveDestination::OBJECTITVE ? blip::get_objective_location() : blip::get_blip_location((int)BlipIcons::Waypoint);

				if (location.has_value())
				{
					auto waypoint = location.value();
					TASK::TASK_VEHICLE_DRIVE_TO_COORD_LONGRANGE(self::ped, self::veh, waypoint.x, waypoint.y, waypoint.z, current_speed, current_driving_flag, 20);
					g_auto_drive.is_auto_driving = true;
				}
				else
				{
					g_auto_drive.auto_drive_destination = AutoDriveDestination::STOPPED;
					has_driving_settings_changed        = false;
				}
			}
			else
			{
				bool interupted = (PAD::IS_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_VEH_MOVE_LEFT_ONLY) || PAD::IS_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_VEH_MOVE_RIGHT_ONLY) || PAD::IS_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_VEH_ACCELERATE) || PAD::IS_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_VEH_BRAKE) || PAD::IS_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_VEH_EXIT) || PAD::IS_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_VEH_HANDBRAKE));

				if (has_driving_settings_changed || interupted)
				{
					TASK::CLEAR_PRIMARY_VEHICLE_TASK(self::veh);
					TASK::CLEAR_PED_TASKS(self::ped);

					has_driving_settings_changed = false;
					g_auto_drive.is_auto_driving = false; // start driving again in next tick if !interupted
				}

				if (interupted)
					g_auto_drive.auto_drive_destination = AutoDriveDestination::STOPPED;
			}
		}
	}
}