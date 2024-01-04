#include "backend/looped_command.hpp"
#include "core/data/vehicle.hpp"
#include "natives.hpp"
#include "util/vehicle.hpp"

namespace big
{
	class vehicle_lock : looped_command
	{
		using looped_command::looped_command;

		CVehicle* m_vehicle = nullptr;
		Vehicle veh;
		int passengers_count = 0;

		void lock_veh()
		{
			g_vehicle.vehicle_lock_unlock_temporarily = false;
			m_vehicle                                 = g_local_player->m_vehicle;

			if (m_vehicle)
			{
				m_vehicle->m_door_lock_status = (int)eVehicleLockState::VEHICLELOCK_LOCKED;
				veh                           = g_pointers->m_gta.m_ptr_to_handle(m_vehicle);
			}
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
						passengers_count              = VEHICLE::GET_VEHICLE_NUMBER_OF_PASSENGERS(veh, 1, 0);
					}

					if (passengers_count != VEHICLE::GET_VEHICLE_NUMBER_OF_PASSENGERS(veh, 1, 0))
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