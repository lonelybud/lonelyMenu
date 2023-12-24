#include "backend/looped/looped.hpp"
#include "core/data/debug.hpp"
#include "core/scr_globals.hpp"
#include "gta/joaat.hpp"
#include "services/tunables/tunables_service.hpp"

namespace big
{
	void looped::self_persist_outfit()
	{
		if (g_debug.disable_clothing_validation)
		{
			*scr_globals::reset_clothing.as<PBOOL>() = FALSE;
			if (auto tunable = g_tunables_service->get_tunable<PBOOL>(RAGE_JOAAT("DISABLE_CLOTHING_SAVE_SLOT_VALIDATION")))
				*tunable = TRUE;
		}
	}
}