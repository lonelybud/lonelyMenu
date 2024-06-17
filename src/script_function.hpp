#pragma once
#include "gta/joaat.hpp"
#include "memory/pattern.hpp"

#include <script/scrProgram.hpp>

namespace big
{
	// a lightweight script function wrapper inspired by https://github.com/Parik27/V.Rainbomizer/blob/master/src/mission/missions_YscUtils.hh
	class script_function
	{
		rage::joaat_t m_script;
		const memory::pattern m_pattern;
		int32_t m_offset;
		int32_t m_ip;
		std::string m_name;

	public:
		script_function(const std::string& name, const rage::joaat_t script, const std::string& pattern, int32_t offset);
		void populate_ip();
		void call(rage::scrThread* thread, rage::scrProgram* program, std::initializer_list<uint64_t> args);
		void call_latent(rage::scrThread* thread, rage::scrProgram* program, std::initializer_list<uint64_t> args, bool& done);

		// for pure functions that do not need access to thread stack
		void static_call(std::initializer_list<uint64_t> args);

		void operator()(std::initializer_list<uint64_t> args);
	};

	namespace scr_functions
	{
		inline script_function reset_session_data("RSD", "pausemenu_multiplayer"_J, "2D 02 7D 00 00", 0);
	}
}
