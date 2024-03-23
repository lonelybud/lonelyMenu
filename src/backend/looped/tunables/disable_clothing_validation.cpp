#include "backend/looped_command.hpp"
#include "core/data/misc.hpp"
#include "core/scr_globals.hpp"
#include "services/notifications/notification_service.hpp"
#include "services/tunables/tunables_service.hpp"

namespace big
{
	class disable_clothing_validation : looped_command
	{
		using looped_command::looped_command;

		bool initialized = false;
		BOOL m_global, m_tunable;
		PBOOL global, tunable;

		virtual void on_enable() override
		{
			tunable = g_tunables_service->get_tunable<PBOOL>("DISABLE_CLOTHING_SAVE_SLOT_VALIDATION"_J);
			global  = scr_globals::reset_clothing.as<PBOOL>();

			if (tunable && global)
			{
				m_global    = *global;
				m_tunable   = *tunable;
				initialized = true;
			}
			else
			{
				initialized                        = false;
				g_misc.disable_clothing_validation = false;
				g_notification_service.push_error("Clothing Validation", "Failed to switch on");
			}
		}

		virtual void on_tick() override
		{
			if (initialized)
			{
				*global  = FALSE;
				*tunable = TRUE;
			}
		}

		virtual void on_disable() override
		{
			if (initialized)
			{
				*global     = m_global;
				*tunable    = m_tunable;
				initialized = false;
			}
		}
	};

	disable_clothing_validation
	    g_disable_clothing_validation("disable_clothing_validation", "Disable Clothing Validation", "", g_misc.disable_clothing_validation);
}
