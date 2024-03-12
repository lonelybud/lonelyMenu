#include "tunables_service.hpp"

#include "gta/joaat.hpp"
#include "gta_util.hpp"
#include "natives.hpp"
#include "script.hpp"
#include "services/script_patcher/script_patcher_service.hpp"
#include "util/globals.hpp"

namespace big
{
	static auto tp_hash = "tuneables_processing"_J;

	tunables_service::tunables_service()
	{
		g_tunables_service = this;
	}

	tunables_service::~tunables_service()
	{
		m_tunables.clear();
		m_running          = false;
		g_tunables_service = nullptr;
	}

	void tunables_service::run_script()
	{
		while (true)
		{
			script::get_current()->yield();

			auto script_running = SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH(tp_hash) != 0;

			if (m_running)
			{
				if (!script_running)
				{
					m_running = false;
					g_script_patcher_service->update();
					LOGF(INFO, "Tunables processing: ended! ({} tunables found)", m_tunables.size());
					return;
				}
			}
			// if script is not already running in the past, start script manually from the starting so that we can get complete tunables
			else if (!script_running)
			{
				SCRIPT::REQUEST_SCRIPT_WITH_NAME_HASH(tp_hash);
				if (SCRIPT::HAS_SCRIPT_WITH_NAME_HASH_LOADED(tp_hash))
				{
					uint64_t args[] = {6, 27};
					if (!SYSTEM::START_NEW_SCRIPT_WITH_NAME_HASH_AND_ARGS(tp_hash, (Any*)args, sizeof(args) / 8, globals::DEFAULT_STACK_SIZE))
					{
						LOG(FATAL) << "Tunables processing: failed to start script!";
						return;
					}
					SCRIPT::SET_SCRIPT_WITH_NAME_HASH_AS_NO_LONGER_NEEDED(tp_hash);
					m_running = true;

					g_script_patcher_service->add_patch({tp_hash, "tuneables_processing1", "2E ? ? 55 ? ? 38 06", 0, std::vector<uint8_t>(17, 0x0), &m_running}); // bool tunables registration hack
					if (auto program = gta_util::find_script_program(tp_hash))
						g_script_patcher_service->on_script_load(program, "tunables_service");

					LOG(INFO) << "Tunables processing: started!";
				}
			}
		}
	}
}
