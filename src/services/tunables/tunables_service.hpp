#pragma once

namespace big
{
#pragma pack(push, 1)
	struct tunable_save_struct
	{
		rage::joaat_t hash;
		uint32_t offset;
	};
#pragma pack(pop)

	class tunables_service
	{
	public:
		tunables_service();
		~tunables_service();
		void run_script();

		inline void register_tunable(rage::joaat_t hash, void* pointer)
		{
			m_tunables.emplace(hash, pointer);
		}

		template<typename T>
		inline std::enable_if_t<std::is_pointer_v<T>, T> get_tunable(rage::joaat_t hash)
		{
			if (auto it = m_tunables.find(hash); it != m_tunables.end())
				return reinterpret_cast<T>(it->second);

			// LOG(WARNING) << "Tunable 0x" << hash << " not found.";

			return nullptr;
		}

	private:
		bool m_running = false;

		std::unordered_map<rage::joaat_t, void*> m_tunables{};
	};

	inline tunables_service* g_tunables_service;
}