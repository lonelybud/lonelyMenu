#include "backend/looped_command.hpp"
#include "core/data/session.hpp"
#include "pointers.hpp"
#include "util/scripts.hpp"

namespace big
{
	class force_freemode_host : looped_command
	{
		using looped_command::looped_command;

		virtual void on_tick() override
		{
			if (g_pointers->m_gta.m_is_session_started && !scripts::is_local_player_host("freemode"_J))
				scripts::force_migration("freemode"_J, true);
		}
	};

	force_freemode_host g_force_freemode_host("force_freemode_host", "", "", g_session.force_freemode_host);
}
