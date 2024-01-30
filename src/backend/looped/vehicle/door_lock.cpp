#include "backend/looped_command.hpp"
#include "core/data/vehicle.hpp"
#include "natives.hpp"
#include "util/vehicle.hpp"

namespace big
{
	static int get_passengers_count()
	{
		auto passengers = g_local_player->m_vehicle->m_driver && g_local_player->m_vehicle->m_driver->m_health ? 1 : 0;
		for (int i = 0; i < g_local_player->m_vehicle->m_max_passengers; ++i)
			if (g_local_player->m_vehicle->m_passengers[i] && g_local_player->m_vehicle->m_passengers[i]->m_health)
				++passengers;

		return passengers;
	}

	class vehicle_lock : looped_command
	{
		using looped_command::looped_command;

		CVehicle* m_vehicle  = nullptr;
		int passengers_count = 0;

		void lock_veh()
		{
			g_vehicle.vehicle_lock_unlock_temporarily = false;
			m_vehicle                                 = g_local_player->m_vehicle;

			if (m_vehicle)
				m_vehicle->m_door_lock_status = (int)eVehicleLockState::VEHICLELOCK_LOCKED;
		}

		virtual void on_enable() override
		{
			lock_veh();
		}

		virtual void on_tick() override
		{
			if (m_vehicle != g_local_player->m_vehicle)
				lock_veh();
			else if (m_vehicle)
			{
				if (g_vehicle.vehicle_lock_unlock_temporarily)
				{
					if ((eVehicleLockState)m_vehicle->m_door_lock_status == eVehicleLockState::VEHICLELOCK_LOCKED)
					{
						m_vehicle->m_door_lock_status = (int)eVehicleLockState::VEHICLELOCK_UNLOCKED;
						passengers_count              = get_passengers_count();
					}

					if (passengers_count != get_passengers_count())
						lock_veh();
				}
				else if ((eVehicleLockState)m_vehicle->m_door_lock_status != eVehicleLockState::VEHICLELOCK_LOCKED)
					lock_veh();
			}
		}

		virtual void on_disable() override
		{
			if (m_vehicle)
				m_vehicle->m_door_lock_status = (int)eVehicleLockState::VEHICLELOCK_UNLOCKED;
			m_vehicle = nullptr;
		}
	};

	vehicle_lock g_vehicle_lock("vehicle_lock", "Keep Vehicle Locked", "", g_vehicle.vehicle_lock);
}