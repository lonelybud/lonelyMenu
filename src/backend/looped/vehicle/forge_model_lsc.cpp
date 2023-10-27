#include "backend/looped_command.hpp"
#include "core/settings/vehicle.hpp"
#include "natives.hpp"

namespace big
{
	class forge_model_lsc : looped_command
	{
		using looped_command::looped_command;

		CVehicle* last_veh;
		int32_t original_model;

		virtual void on_tick() override
		{
			auto* const vehicle = g_local_player->m_vehicle;

			if (last_veh != vehicle)
			{
				if (last_veh)
					last_veh->m_model_info->m_hash = original_model;

				last_veh = vehicle;

				if (vehicle)
				{
					original_model                = vehicle->m_model_info->m_hash;
					vehicle->m_model_info->m_hash = 0x3412AE2D;
				}
			}
		}

		virtual void on_disable() override
		{
			if (last_veh)
			{
				last_veh->m_model_info->m_hash = original_model;
				last_veh                       = nullptr;
			}
		}
	};

	forge_model_lsc g_forge_model_lsc("forge_model_lsc", "Forge Model for LSC", "Allow hot cars in LSC", g_vehicle.forge_model_lsc);
}
