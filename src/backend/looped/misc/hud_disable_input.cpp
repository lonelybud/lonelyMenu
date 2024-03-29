#include "core/data/hud.hpp"
#include "natives.hpp"
#include "backend/looped/looped.hpp"

namespace big
{
	void looped::hud_disable_input()
	{
		if (g_hud.typing)
		{
			PAD::DISABLE_ALL_CONTROL_ACTIONS(0);
			g_hud.typing--;
		}
	}
}