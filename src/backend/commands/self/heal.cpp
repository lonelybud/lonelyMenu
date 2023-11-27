#include "backend/command.hpp"
#include "natives.hpp"
#include "util/ped.hpp"

namespace big
{
	class heal : command
	{
		using command::command;

		virtual void execute() override
		{
			auto ped = ped::get_self_ped();
			ENTITY::SET_ENTITY_HEALTH(ped, PED::GET_PED_MAX_HEALTH(ped), 0);
			PED::SET_PED_ARMOUR(ped, PLAYER::GET_PLAYER_MAX_ARMOUR(self::id));
		}
	};

	heal g_heal("heal", "Heal", "Restores full health and armor");
}