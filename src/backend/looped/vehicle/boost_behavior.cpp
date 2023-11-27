#include "backend/looped_command.hpp"
#include "core/data/vehicle.hpp"
#include "natives.hpp"

namespace big
{
	class veh_boost : looped_command
	{
		using looped_command::looped_command;

		virtual void on_tick() override
		{
			if (auto* const vehicle = g_local_player->m_vehicle)
			{
				bool is_rocket = VEHICLE::GET_HAS_ROCKET_BOOST(self::veh);

				if (is_rocket && (vehicle->m_boost == 0.f || !vehicle->m_boost_state))
				{
					vehicle->m_boost_allow_recharge = true;
					vehicle->m_boost                = 3.f;
				}
			}
		}
	};

	veh_boost g_veh_boost("veh_boost", "Rocket boost Auto-Recharge", "", g_vehicle.veh_boost);
}
