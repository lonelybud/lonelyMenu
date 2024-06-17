#pragma once

namespace big
{
	inline auto get_current_time_in_mill()
	{
		auto currentTime = std::chrono::system_clock::now().time_since_epoch();
		return std::chrono::duration_cast<std::chrono::milliseconds>(currentTime).count();
	}
}