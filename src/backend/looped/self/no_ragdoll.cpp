#include "backend/bool_command.hpp"
#include "core/data/self.hpp"
#include "natives.hpp"

namespace big
{
	class no_ragdoll : bool_command
	{
		using bool_command::bool_command;

		virtual void on_enable() override
		{
			PED::SET_PED_CAN_RAGDOLL(self::ped, false);
		}

		virtual void on_disable() override
		{
			PED::SET_PED_CAN_RAGDOLL(self::ped, true);
		}
	};

	no_ragdoll g_no_ragdoll("noragdoll", "No Ragdoll", "Prevents you from ragdolling", g_self.no_ragdoll);
}
