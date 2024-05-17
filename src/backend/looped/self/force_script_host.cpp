#include "backend/looped_command.hpp"
#include "core/data/session.hpp"
#include "pointers.hpp"
#include "util/scripts.hpp"

namespace big
{
	template<rage::joaat_t hash>
	class force_script_host : looped_command
	{
		using looped_command::looped_command;

		virtual void on_tick() override
		{
			auto comp = scripts::get_script_handler_net_comp(hash);
			if (g_pointers->m_gta.m_is_session_started && comp && !comp->is_local_player_host())
				scripts::force_migration(hash, true);
		}
	};

	force_script_host<"freemode"_J> g_fsc_freemode("force_freemode_host", "", "", g_session.force_freemode_host);
	force_script_host<"fmmc_launcher"_J> g_fsc_fmmc("force_fmmc_host", "", "", g_session.force_fmmc_launcher_host);
	force_script_host<"am_launcher"_J> g_fsc_am("force_am_host", "", "", g_session.force_am_launcher_host);
}