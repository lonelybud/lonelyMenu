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
			for (const auto& [_, weapon] : g_gta_data_service->weapons())
			{
				int ammo_in;
				auto ped = ped::get_self_ped();
				WEAPON::GET_MAX_AMMO(ped, weapon.m_hash, &ammo_in);
				WEAPON::SET_PED_AMMO(ped, weapon.m_hash, ammo_in, 0);
			}
		}
	};
	fill_ammo g_fill_ammo("fillammo", "Fill Ammo", "Fills all of your ammo.", 0);
}