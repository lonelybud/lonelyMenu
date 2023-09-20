#include "backend/looped/looped.hpp"
#include "natives.hpp"
#include "services/context_menu/context_menu_service.hpp"

#include <gta/enums.hpp>

namespace big
{
	static const ControllerInputs attack_controls[] = {
	    ControllerInputs::INPUT_NEXT_WEAPON,
	    ControllerInputs::INPUT_PREV_WEAPON,
	    ControllerInputs::INPUT_VEH_NEXT_RADIO,
	    ControllerInputs::INPUT_VEH_SELECT_NEXT_WEAPON,
	    ControllerInputs::INPUT_SELECT_NEXT_WEAPON,
	    ControllerInputs::INPUT_SELECT_PREV_WEAPON,
	    ControllerInputs::INPUT_ATTACK,
	    ControllerInputs::INPUT_ATTACK2,
	    ControllerInputs::INPUT_SPECIAL_ABILITY,
	    ControllerInputs::INPUT_VEH_MOUSE_CONTROL_OVERRIDE,
	    ControllerInputs::INPUT_SNIPER_ZOOM,
	};

	void looped::disable_attack_actions()
	{
		bool is_custom_gun_selected = g.weapons.custom_weapon != CustomWeapon::NONE;
		if (g_context_menu_service->enabled || is_custom_gun_selected || isVehicleLifted)
		{
			for (const auto& control : attack_controls)
				PAD::DISABLE_CONTROL_ACTION(0, static_cast<int>(control), true);
		}
	}
}
