#include "backend/looped_command.hpp"
#include "core/data/self.hpp"
#include "services/notifications/notification_service.hpp"
#include "services/tunables/tunables_service.hpp"

namespace big
{
	class no_idle_kick : looped_command
	{
		using looped_command::looped_command;

		bool initialized = false;
		std::array<int, 8> m_restore;

		const std::vector<Hash> m_tunable_hashes = {"IDLEKICK_WARNING1"_J, "IDLEKICK_WARNING2"_J, "IDLEKICK_WARNING3"_J, "IDLEKICK_KICK"_J, "ConstrainedKick_Warning1"_J, "ConstrainedKick_Warning2"_J, "ConstrainedKick_Warning3"_J, "ConstrainedKick_Kick"_J};

		virtual void on_enable() override
		{
			// create backup of tunables
			for (int i = 0; i < m_restore.size(); ++i)
				if (auto tunable = g_tunables_service->get_tunable<int*>(m_tunable_hashes[i]))
					m_restore[i] = *tunable;
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
				for (auto hash_iter : m_tunable_hashes)
					*g_tunables_service->get_tunable<int*>(hash_iter) = INT_MAX;
		}

		virtual void on_disable() override
		{
			if (initialized)
			{
				for (int i = 0; i < m_restore.size(); ++i)
					*g_tunables_service->get_tunable<int*>(m_tunable_hashes[i]) = m_restore[i];

				initialized = false;
			}
		}
	};

	no_idle_kick g_no_idle_kick("noidlekick", "No Idle Kick", "", g_self.no_idle_kick);
}
