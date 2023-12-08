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

		CVehicle* m_vehicle           = nullptr;
		uint16_t m_original_abilities = 0;

		void reset_defaults()
		{
			if (m_vehicle && m_vehicle->m_model_info)
			{
				reinterpret_cast<CVehicleModelInfo*>(m_vehicle->m_model_info)->m_ability_flag = m_original_abilities;
				m_vehicle                                                                     = nullptr;
				m_original_abilities                                                          = 0;
			}
		}

		inline void setup(CVehicle* vehicle)
		{
			if (vehicle && vehicle->m_model_info)
			{
				m_original_abilities = reinterpret_cast<CVehicleModelInfo*>(vehicle->m_model_info)->m_ability_flag;

				auto ability = (uint16_t)eVehicleAbilities::ROCKET;

				if ((m_original_abilities >> ability) & 1)
				{
					m_original_abilities   = 0;
					g_vehicle.force_rocket = false;
					g_notification_service->push_success("Force Vehicle Boost", "The vehicle already has this ability.");
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
					reset_defaults();

				return;
			}

			if (m_vehicle != curr_veh)
			{
				if (m_vehicle)
					reset_defaults();

				setup(curr_veh);
			}

			if (m_vehicle && m_vehicle->m_model_info)
			{
				const auto model_info = reinterpret_cast<CVehicleModelInfo*>(m_vehicle->m_model_info);
				misc::set_bits(&model_info->m_ability_flag, (uint16_t)eVehicleAbilities::ROCKET);
			}
		}

		virtual void on_disable() override
		{
			reset_defaults();
		}
	};

	vehicle_ability g_rocket_ability("rocketability", "Force Rocket Ability", "Rocket Voltic's booster", g_vehicle.force_rocket);
}