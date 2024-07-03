#include "tunables_service.hpp"

#include "gta/joaat.hpp"
#include "gta_util.hpp"
#include "natives.hpp"
#include "script.hpp"
#include "services/script_patcher/script_patcher_service.hpp"
#include "util/globals.hpp"

namespace big
{
	static auto tp_hash                       = "tuneables_processing"_J;
	static auto tr_hash                       = "tunables_registration"_J;
	static constexpr auto tunable_global_base = 0x40000;

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

			// tunable processing in progress
			if (m_running)
			{
				// tunable processing triggered by us has completed
				if (!script_running)
				{
					// save the tunable hash and its correspoding script global int in "m_tunables"
					// from modified memory region created by us during tunable processing phase
					for (int i = 0; i < m_num_tunables; i++)
					{
						auto value = *script_global(tunable_global_base).at(i).as<int*>();
						if (auto it = m_junk_values.find(value); it != m_junk_values.end())
							m_tunables.emplace(it->second, tunable_global_base + i);
					}

					// restore original tunable memory state
					memcpy(script_global(tunable_global_base).as<void*>(), m_tunables_backup.get(), m_num_tunables * 8);

					// mark the end
					m_tunables_backup.release();
					m_running = false;
					LOGF(INFO, "Tunables processing: ended! ({} tunables found)", m_tunables.size());
					return;
				}
			}
			// if script is not already running in the past, start script manually from the starting so that we can get complete tunables
			else if (!script_running)
			{
				SCRIPT::REQUEST_SCRIPT_WITH_NAME_HASH(tp_hash);
				SCRIPT::REQUEST_SCRIPT_WITH_NAME_HASH(tr_hash);

				if (SCRIPT::HAS_SCRIPT_WITH_NAME_HASH_LOADED(tp_hash) && SCRIPT::HAS_SCRIPT_WITH_NAME_HASH_LOADED(tr_hash))
				{
					// get tunables count
					m_num_tunables = gta_util::find_script_program(tr_hash)->m_global_count - tunable_global_base;
					LOGF(INFO, "Tunables processing: preparing! ({} tunables found)", m_num_tunables);

					// start tunable script to start tunables processing phase
					uint64_t args[] = {6, 27};
					if (!SYSTEM::START_NEW_SCRIPT_WITH_NAME_HASH_AND_ARGS(tp_hash, (Any*)args, sizeof(args) / 8, globals::DEFAULT_STACK_SIZE))
					{
						LOG(FATAL) << "Tunables processing: failed to start script!";
						return;
					}

					// backup original tunable memory state since it will be filled with junk values during processing phase
					m_tunables_backup = std::make_unique<std::uint64_t[]>(m_num_tunables);
					memcpy(m_tunables_backup.get(), script_global(tunable_global_base).as<void*>(), m_num_tunables * 8);

					// mark tunable processing in progress
					m_running = true;
					LOG(INFO) << "Tunables processing: started!";

					// mark loaded scripts as garbage collection
					SCRIPT::SET_SCRIPT_WITH_NAME_HASH_AS_NO_LONGER_NEEDED(tp_hash);
					SCRIPT::SET_SCRIPT_WITH_NAME_HASH_AS_NO_LONGER_NEEDED(tr_hash);
				}
			}
		}
	}
}
