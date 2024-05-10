#include "backend/looped_command.hpp"
#include "core/data/self.hpp"
#include "util/local_player.hpp"

namespace big
{
	class never_wanted : looped_command
	{
		using looped_command::looped_command;

		virtual void on_tick() override
		{
			local_player::remove_wanted();
		}
	};

	never_wanted g_never_wanted("never_wanted", "Never Wanted", "", g_self.never_wanted);
}