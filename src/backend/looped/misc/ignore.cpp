#include "backend/looped_command.hpp"
#include "core/data/misc.hpp"
#include "natives.hpp"
#include "util/entity.hpp"
#include "core/vars.hpp"

namespace big
{
	class ignore : looped_command
	{
		using looped_command::looped_command;

		virtual void on_enable() override
		{
			PLAYER::SET_EVERYONE_IGNORE_PLAYER(self::id, TRUE);
			PLAYER::SET_POLICE_IGNORE_PLAYER(self::id, TRUE);
		}

		virtual void on_tick() override
		{
			for (auto ped : entity::get_entities(false, true))
				if (!PED::IS_PED_A_PLAYER(ped) && !PED::GET_PED_CONFIG_FLAG(ped, 17, TRUE))
				{
					PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, TRUE);
					TASK::TASK_SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, TRUE);
				}
		}

		virtual void on_disable() override
		{
			PLAYER::SET_EVERYONE_IGNORE_PLAYER(self::id, FALSE);
			PLAYER::SET_POLICE_IGNORE_PLAYER(self::id, FALSE);
		}
	};

	ignore g_ignore("pedsignore", "Ped Ignore", "Everyone ignores you", g_misc.ped_ignore);
}