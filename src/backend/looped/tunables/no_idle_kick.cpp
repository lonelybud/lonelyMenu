#include "backend/looped_command.hpp"
#include "core/data/self.hpp"
#include "services/notifications/notification_service.hpp"
#include "services/tunables/tunables_service.hpp"

namespace big
{
	class no_idle_kick : looped_command
	{
		using looped_command::looped_command;

		bool initialized               = false;
		std::array<int*, 8> m_tunables = {nullptr};
		std::array<int, 8> m_restore;

		virtual void on_enable() override
		{
			m_tunables[0] = g_tunables_service->get_tunable<int*>("IDLEKICK_WARNING1"_J);
			m_tunables[1] = g_tunables_service->get_tunable<int*>("IDLEKICK_WARNING2"_J);
			m_tunables[2] = g_tunables_service->get_tunable<int*>("IDLEKICK_WARNING3"_J);
			m_tunables[3] = g_tunables_service->get_tunable<int*>("IDLEKICK_KICK"_J);
			m_tunables[4] = g_tunables_service->get_tunable<int*>("ConstrainedKick_Warning1"_J);
			m_tunables[5] = g_tunables_service->get_tunable<int*>("ConstrainedKick_Warning2"_J);
			m_tunables[6] = g_tunables_service->get_tunable<int*>("ConstrainedKick_Warning3"_J);
			m_tunables[7] = g_tunables_service->get_tunable<int*>("ConstrainedKick_Kick"_J);

			// create backup of tunables
			for (int i = 0; i < m_restore.size(); ++i)
				if (m_tunables[i])
					m_restore[i] = *m_tunables[i];
				else
				{
					initialized         = false;
					g_self.no_idle_kick = false;
					g_notification_service.push_error("Idle Kick", "Failed to switch on");
					return;
				}

			initialized = true;
		}


		virtual void on_tick() override
		{
			if (initialized)
				for (const auto& tunable : m_tunables)
					*tunable = INT_MAX;
		}

		virtual void on_disable() override
		{
			if (initialized)
			{
				for (int i = 0; i < m_restore.size(); ++i)
					*m_tunables[i] = m_restore[i];

				initialized = false;
			}
		}
	};

	no_idle_kick g_no_idle_kick("noidlekick", "No Idle Kick", "", g_self.no_idle_kick);
}
