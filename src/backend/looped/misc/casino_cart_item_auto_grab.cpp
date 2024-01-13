#include "backend/looped_command.hpp"
#include "core/data/misc.hpp"
#include "natives.hpp"
#include "util/lua_script.hpp"

namespace big
{
	constexpr int fm_mission_controller_cart_grab       = 10253;
	constexpr int fm_mission_controller_cart_grab_speed = 14;

	class casino_cart_item_auto_grab : looped_command
	{
		using looped_command::looped_command;

		virtual void on_tick() override
		{
			if (lua_script::locals::get_int("fm_mission_controller", fm_mission_controller_cart_grab) == 3)
				lua_script::locals::set_int("fm_mission_controller", fm_mission_controller_cart_grab, 4);
			else if (lua_script::locals::get_int("fm_mission_controller", fm_mission_controller_cart_grab) == 4)
				lua_script::locals::set_float("fm_mission_controller", fm_mission_controller_cart_grab + fm_mission_controller_cart_grab_speed, 2);
		}

		virtual void on_disable() override
		{
			NETWORK::NETWORK_CLEAR_CLOCK_TIME_OVERRIDE();
		}
	};

	casino_cart_item_auto_grab
	    g_casino_cart_item_auto_grab("casino_cart_item_auto_grab", "Auto Grab Cart Item", "", g_misc.fm_mission_controller_cart_grab);
}
