#include "backend/looped_command.hpp"
#include "core/data/lua_scripts.hpp"
#include "core/data/misc.hpp"
#include "natives.hpp"

namespace big
{
	class casino_cart_item_auto_grab : looped_command
	{
		using looped_command::looped_command;

		virtual void on_tick() override
		{
			lua_scripts::casino_cart_item_auto_grab();
		}
	};

	casino_cart_item_auto_grab
	    g_casino_cart_item_auto_grab("casino_cart_item_auto_grab", "Auto Grab Cart Item", "", g_misc.fm_mission_controller_cart_grab);
}
