#include "backend/looped_command.hpp"
#include "core/data/vehicle.hpp"
#include "core/enums.hpp"
#include "gta/enums.hpp"
#include "services/notifications/notification_service.hpp"
#include "util/misc.hpp"

#include <vehicle/CVehicleModelInfo.hpp>

namespace big
{
	class vehicle_ability : looped_command
	{
		using looped_command::looped_command;

		CVehicle* m_vehicle = nullptr;

		inline bool has_ability(CVehicle* vehicle, CVehicleModelInfoFlags ability)
		{
			return reinterpret_cast<CVehicleModelInfo*>(vehicle->m_model_info)->get_vehicle_model_flag(ability);
		}

		inline void toggle_ability(bool state)
		{
			reinterpret_cast<CVehicleModelInfo*>(m_vehicle->m_model_info)->set_vehicle_model_flag(CVehicleModelInfoFlags::HAS_ROCKET_BOOST, state);
		}

		inline void setup(CVehicle* vehicle)
		{
			if (vehicle && vehicle->m_model_info)
			{
				if (has_ability(vehicle, CVehicleModelInfoFlags::HAS_ROCKET_BOOST))
				{
					g_vehicle.force_rocket = false;
					g_notification_service->push_error("Force Vehicle Boost", "The vehicle already has this ability.");
					return;
				}

				m_vehicle = vehicle;
			}
		}

		virtual void on_enable() override
		{
			setup(g_local_player->m_vehicle);
		}

		virtual void on_tick() override
		{
			const auto curr_veh = g_local_player->m_vehicle;

			if (!(g_local_player->m_ped_task_flag & (int)ePedTask::TASK_DRIVING))
			{
				if (m_vehicle)
					on_disable();

				return;
			}

			if (m_vehicle != curr_veh)
			{
				if (m_vehicle)
					on_disable();

				setup(curr_veh);
			}

			if (m_vehicle && m_vehicle->m_model_info)
				toggle_ability(true);
		}

		virtual void on_disable() override
		{
			if (m_vehicle && m_vehicle->m_model_info)
			{
				toggle_ability(false);
				m_vehicle = nullptr;
			}
		}
	};

	vehicle_ability g_rocket_ability("rocketability", "Force Rocket Ability", "Rocket Voltic's booster", g_vehicle.force_rocket);
}