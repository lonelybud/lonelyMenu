#pragma once
#include "logger/logger.hpp"
#include "script_global.hpp"

namespace big
{
	class tunables_service
	{
		bool m_running = false;

		// tunable name hash, script global int
		std::unordered_map<rage::joaat_t, int> m_tunables{};

		// variables usefull during tunables processing and useless afterwards
		std::unique_ptr<uint64_t[]> m_tunables_backup;
		int m_num_tunables;

	public:
		// variables usefull during tunables processing and useless afterwards
		int m_current_junk_val = 0x1000000;
		std::unordered_map<int, rage::joaat_t> m_junk_values{};

		tunables_service();
		~tunables_service();
		void run_script();

		template<typename T>
		inline std::enable_if_t<std::is_pointer_v<T>, T> get_tunable(rage::joaat_t hash)
		{
			if (auto it = m_tunables.find(hash); it != m_tunables.end())
				return reinterpret_cast<T>(script_global(it->second).as<void*>());

			// LOG(WARNING) << "Tunable 0x" << hash << " not found.";

			return nullptr;
		}

		// wrapper around get_tunable(), may not set the tunable immediately if the service isn't initialized yet
		template<typename T>
		inline void set_tunable(rage::joaat_t hash, T value)
		{
			if (auto tunable = get_tunable<T*>(hash))
				*tunable = value;
			else
				LOG(WARNING) << "Tunable 0x" << hash << " not found.";
		}

		inline bool is_processing()
		{
			return m_running;
		}
	};

	inline tunables_service* g_tunables_service;
}