#include "backend/command.hpp"
#include "natives.hpp"
#include "services/gta_data/gta_data_service.hpp"
#include "util/ped.hpp"

namespace big
{
	class fill_ammo : command
	{
		using command::command;

		virtual void execute(const command_arguments&, const std::shared_ptr<command_context> ctx) override
		{
			int ammo_in;
			Hash weapon_hash;
			auto ped = ped::get_self_ped();
			
			WEAPON::GET_CURRENT_PED_WEAPON(ped, &weapon_hash, NULL);
			WEAPON::GET_MAX_AMMO(ped, weapon_hash, &ammo_in);
			WEAPON::SET_PED_AMMO(ped, weapon_hash, ammo_in, 0);
		}
	};
	fill_ammo g_fill_ammo("fillammo", "Fill Ammo", "Fills ammo for current weapon", 0);
}