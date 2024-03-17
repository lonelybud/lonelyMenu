#pragma once
#include "gta_util.hpp"
#include "local_player.hpp"
#include "natives.hpp"
#include "script_global.hpp"
#include "script_local.hpp"

namespace big::lua_helper
{
	static Hash stat_text_to_hash(std::string& text)
	{
		if (text[0] == '$')
		{
			text = text.substr(1);
		}

		std::transform(text.begin(), text.end(), text.begin(), ::toupper);

		if (text.substr(0, 3) == "MPX")
			text[2] = big::local_player::get_active_character_slot() + '0';

		return rage::joaat(text);
	}

	namespace stats
	{
		inline int get_int(Hash stat_hash)
		{
			int out_value;
			STATS::STAT_GET_INT(stat_hash, &out_value, -1);

			return out_value;
		}
		inline int get_int(std::string stat_name)
		{
			return get_int(stat_text_to_hash(stat_name));
		}

		inline bool set_int(Hash stat_hash, int value, bool save = true)
		{
			return STATS::STAT_SET_INT(stat_hash, value, save);
		}
		inline bool set_int(std::string stat_name, int value, bool save = true)
		{
			return set_int(stat_text_to_hash(stat_name), value, save);
		}

		inline bool set_bool_masked(Hash stat_hash, bool new_value, int bit_index)
		{
			constexpr int bit_count = 8;
			return STATS::STAT_SET_MASKED_INT(stat_hash, new_value, bit_index, bit_count, true);
		}
		inline bool set_bool_masked(std::string stat_name, bool new_value, int bit_index)
		{
			return set_bool_masked(stat_text_to_hash(stat_name), new_value, bit_index);
		}

		static bool set_masked_int(Hash stat_hash, int new_value, int bit_start, int bit_size)
		{
			return STATS::STAT_SET_MASKED_INT(stat_hash, new_value, bit_start, bit_size, true);
		}
		static bool set_masked_int(std::string stat_name, int new_value, int bit_start, int bit_size)
		{
			return set_masked_int(stat_text_to_hash(stat_name), new_value, bit_start, bit_size);
		}

		static int get_packed_stat_int(int index)
		{
			return STATS::GET_PACKED_STAT_INT_CODE(index, big::local_player::get_active_character_slot());
		}
		static bool get_packed_stat_bool(int index)
		{
			return STATS::GET_PACKED_STAT_BOOL_CODE(index, big::local_player::get_active_character_slot());
		}
	}

	namespace locals
	{
		template<typename T>
		inline T get(const std::string& script, int index)
		{
			static std::remove_pointer_t<T> null{};
			auto thread = gta_util::find_script_thread(rage::joaat(script));
			if (thread)
				return script_local(thread->m_stack, index).as<T>();

			return &null;
		}
		inline int get_int(const std::string& script, int index)
		{
			return *get<int*>(script, index);
		}

		inline void set_int(const std::string& script, int index, int val)
		{
			*get<int*>(script, index) = val;
		}

		inline int get_float(const std::string& script, int index)
		{
			return *get<float*>(script, index);
		}
		inline void set_float(const std::string& script, int index, float val)
		{
			*get<float*>(script, index) = val;
		}
	}

	namespace globals
	{
		inline int get_int(int global)
		{
			return *big::script_global(global).as<int*>();
		}
		inline void set_int(int global, int val)
		{
			*big::script_global(global).as<int*>() = val;
		}
	}
}
