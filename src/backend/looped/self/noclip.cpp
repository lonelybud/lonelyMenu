#include "backend/looped_command.hpp"
#include "core/data/self.hpp"
#include "core/vars.hpp"
#include "gta/enums.hpp"
#include "natives.hpp"
#include "util/vehicle.hpp"

namespace big
{
	static constexpr ControllerInputs controls[] = {ControllerInputs::INPUT_SPRINT, ControllerInputs::INPUT_MOVE_UP_ONLY, ControllerInputs::INPUT_MOVE_DOWN_ONLY, ControllerInputs::INPUT_MOVE_LEFT_ONLY, ControllerInputs::INPUT_MOVE_RIGHT_ONLY, ControllerInputs::INPUT_DUCK};

	static constexpr float speed = 1.0f;

	class noclip : looped_command
	{
		using looped_command::looped_command;

		bool entity_freezed = false;

		virtual void on_enable() override
		{
			if (self::veh)
			{
				g_self.noclip = false;
				return;
			}
		}

		virtual void on_tick() override
		{
			for (const auto& control : controls)
				PAD::DISABLE_CONTROL_ACTION(0, static_cast<int>(control), true);

			Vector3 vel{};

			// Left Shift
			if (PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_SPRINT))
				vel.z += speed / 2;
			// space
			if (PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_JUMP))
				vel.z -= speed / 2;
			// Forward
			if (PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_MOVE_UP_ONLY))
				vel.y += speed;
			// Backward
			if (PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_MOVE_DOWN_ONLY))
				vel.y -= speed;
			// Left
			if (PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_MOVE_LEFT_ONLY))
				vel.x -= speed;
			// Right
			if (PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_MOVE_RIGHT_ONLY))
				vel.x += speed;

			auto rot = CAM::GET_GAMEPLAY_CAM_ROT(2);
			ENTITY::SET_ENTITY_ROTATION(self::ped, 0.f, rot.y, rot.z, 2, 0);
			ENTITY::SET_ENTITY_COLLISION(self::ped, false, false);

			if (vel.x == 0.f && vel.y == 0.f && vel.z == 0.f)
			{
				ENTITY::FREEZE_ENTITY_POSITION(self::ped, true);
			}
			else
			{
				ENTITY::FREEZE_ENTITY_POSITION(self::ped, false);

				vel = vel * g_self.noclip_speed_multiplier;
				const auto offset = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(self::ped, vel.x, vel.y, 0.f);
				vel.x             = offset.x - self::pos.x;
				vel.y             = offset.y - self::pos.y;

				ENTITY::SET_ENTITY_VELOCITY(self::ped, vel.x, vel.y, vel.z);
			}
		}

		virtual void on_disable() override
		{
			ENTITY::SET_ENTITY_COLLISION(self::ped, true, false);
			ENTITY::FREEZE_ENTITY_POSITION(self::ped, false);
		}
	};

	noclip g_noclip("noclip", "No Clip", "Works only on ped", g_self.noclip);
}
