#include "backend/looped/looped.hpp"
#include "gta/enums.hpp"
#include "gui.hpp"
#include "natives.hpp"
#include "services/context_menu/context_menu_service.hpp"
#include "util/math.hpp"

namespace big
{
	void looped::throw_lifted_vehicle()
	{
		if (isVehicleLifted && !g_gui->is_open() && PAD::IS_DISABLED_CONTROL_JUST_PRESSED(0, (int)ControllerInputs::INPUT_ATTACK))
		{
			Vector3 location = self::pos;

			Vector3 rot = CAM::GET_GAMEPLAY_CAM_ROT(2);
			float pitch = math::deg_to_rad(rot.x);      // vertical
			float yaw   = math::deg_to_rad(rot.z + 90); // horizontal
			float dist  = 15.f;

			float h1 = ENTITY::GET_ENTITY_HEIGHT_ABOVE_GROUND(self::ped);
			float h2 = ENTITY::GET_ENTITY_HEIGHT_ABOVE_GROUND(lifted_vehicle);

			ENTITY::ATTACH_ENTITY_TO_ENTITY(lifted_vehicle, self::ped, ENTITY::GET_ENTITY_BONE_INDEX_BY_NAME(self::ped, "BONETAG_SPINE_ROOT"), h1 + h2, 0, 0, 0, 90, 0, 1, 0, 1, 0, 0, 1, 0);
			TASK::CLEAR_PED_TASKS(self::ped);
			ENTITY::DETACH_ENTITY(lifted_vehicle, 1, 1);

			Vector3 velocity{dist * cos(pitch) * cos(yaw), dist * sin(yaw) * cos(pitch), dist * sin(pitch)};

			ENTITY::SET_ENTITY_VELOCITY(lifted_vehicle, velocity.x, velocity.y, velocity.z);

			isVehicleLifted = false;
		}
	}
}
