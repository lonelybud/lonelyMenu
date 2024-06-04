#include "backend/looped_command.hpp"
#include "core/data/vehicle.hpp"
#include "natives.hpp"
#include "core/vars.hpp"

namespace big
{
	class veh_boost : looped_command
	{
		using looped_command::looped_command;

		virtual void on_tick() override
		{
			if (g_local_player == nullptr)
				return;

			if (auto* const vehicle = g_local_player->m_vehicle)
			{
				bool is_rocket = VEHICLE::GET_HAS_ROCKET_BOOST(self::veh);
				bool is_kers   = VEHICLE::GET_VEHICLE_HAS_KERS(self::veh);

				if (is_rocket && (vehicle->m_boost == 0.f || !vehicle->m_boosting))
				{
					vehicle->m_boost_allow_recharge = true;
					vehicle->m_boost                = 10.f;
				}
				else if (is_kers && vehicle->m_kers_boost == 0.f)
				{
					vehicle->m_kers_boost = vehicle->m_kers_boost_max;
				}
			}
		}
	};

	veh_boost g_veh_boost("veh_boost", "Boost Auto-Recharge", "Instantly refill rocket or kers boost", g_vehicle.veh_boost);
}
