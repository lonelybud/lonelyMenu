#include "backend/looped_command.hpp"
#include "core/settings/vehicle.hpp"
#include "natives.hpp"

namespace big
{
	class keep_engine_running : looped_command
	{
		using looped_command::looped_command;

		virtual void on_tick() override
		{
			if (ENTITY::DOES_ENTITY_EXIST(self::veh) && !VEHICLE::GET_IS_VEHICLE_ENGINE_RUNNING(self::veh))
			{
				VEHICLE::SET_VEHICLE_KEEP_ENGINE_ON_WHEN_ABANDONED(self::veh, true);
			}
		}

		virtual void on_disable() override
		{
			if (ENTITY::DOES_ENTITY_EXIST(self::veh) && VEHICLE::GET_IS_VEHICLE_ENGINE_RUNNING(self::veh))
			{
				VEHICLE::SET_VEHICLE_KEEP_ENGINE_ON_WHEN_ABANDONED(self::veh, false);
			}
		}
	};

	keep_engine_running g_keep_engine_running("keepengine", "Keep Engine Running", "Keeps the engine running when you exit the vehicle",
	    g_vehicle.keep_engine_running);
}