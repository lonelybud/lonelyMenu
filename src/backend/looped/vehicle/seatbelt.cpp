#include "backend/bool_command.hpp"
#include "core/data/vehicle.hpp"
#include "gta/enums.hpp"
#include "natives.hpp"

namespace big
{
	class seatbelt : bool_command
	{
		using bool_command::bool_command;

		virtual void on_enable() override
		{
			PED::SET_PED_CONFIG_FLAG(self::ped, 32, false);
			PED::SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(self::ped, (int)eKnockOffVehicle::KNOCKOFFVEHICLE_NEVER);
		}

		virtual void on_disable() override
		{
			PED::SET_PED_CONFIG_FLAG(self::ped, 32, true);
			PED::SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(self::ped, (int)eKnockOffVehicle::KNOCKOFFVEHICLE_DEFAULT);
		}
	};

	seatbelt g_seatbelt("seatbelt", "Seatbelt", "Prevent you from falling off bikes or flying through the windshield",
	    g_vehicle.seatbelt);
}
