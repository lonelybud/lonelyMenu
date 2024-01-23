#include "backend/looped_command.hpp"
#include "core/data/misc.hpp"
#include "core/scr_globals.hpp"
#include "services/tunables/tunables_service.hpp"

namespace big
{
	class disable_clothing_validation : looped_command
	{
		using looped_command::looped_command;

		bool initialized = false;
		BOOL m_global, m_tunable;

		virtual void on_tick() override
		{
			if (auto tunable = g_tunables_service->get_tunable<PBOOL>(RAGE_JOAAT("DISABLE_CLOTHING_SAVE_SLOT_VALIDATION")))
			{
				if (!initialized)
				{
					m_global    = *scr_globals::reset_clothing.as<PBOOL>();
					m_tunable   = *tunable;
					initialized = true;
				}
				else
				{
					*scr_globals::reset_clothing.as<PBOOL>() = FALSE;
					*tunable                                 = TRUE;
				}
			}
		}

		virtual void on_disable() override
		{
			if (initialized)
			{
				initialized                              = false;
				*scr_globals::reset_clothing.as<PBOOL>() = m_global;
				if (auto tunable = g_tunables_service->get_tunable<PBOOL>(RAGE_JOAAT("DISABLE_CLOTHING_SAVE_SLOT_VALIDATION")))
					*tunable = m_tunable;
			}
		}
	};

	disable_clothing_validation
	    g_disable_clothing_validation("disable_clothing_validation", "Disable Clothing Validation", "", g_misc.disable_clothing_validation);
}
