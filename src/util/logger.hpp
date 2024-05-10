#pragma once
#include "file_manager.hpp"

namespace big
{
	template<typename TP>
	std::time_t to_time_t(TP tp)
	{
		using namespace std::chrono;
		auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now() + system_clock::now());
		return system_clock::to_time_t(sctp);
	}

	inline void logger_create_backup(file& m_file, const char* folder, bool move_file = true)
	{
		if (m_file.exists())
		{
			auto file_time  = std::filesystem::last_write_time(m_file.get_path());
			auto time_t     = to_time_t(file_time);
			auto local_time = std::localtime(&time_t);

			auto file_path = std::format("./{}/{:0>2}-{:0>2}-{}-{:0>2}-{:0>2}-{:0>2}_{}",
			    folder,
			    local_time->tm_mon + 1,
			    local_time->tm_mday,
			    local_time->tm_year + 1900,
			    local_time->tm_hour,
			    local_time->tm_min,
			    local_time->tm_sec,
			    m_file.get_path().filename().string().c_str());

			if (move_file)
				m_file.move(file_path);
			else
				m_file.copy(file_path);
		}
	}
}