#pragma once
#include "file_manager.hpp"
#include "logger/logger.hpp"
#include "util/strings.hpp"

static constexpr int chat_buffer_size         = 8192;
static constexpr int chat_buffer_content_size = chat_buffer_size - 1;
static constexpr int msg_size                 = 320;

namespace big
{
	class custom_chat_buffer
	{
		int len                                         = 0;
		int length_flushed                              = 0;
		std::chrono::system_clock::time_point last_time = std::chrono::system_clock::time_point::min();

		inline void reset_buf()
		{
			strcpy(buf, "");
			len = length_flushed = 0;
			overflow             = false;
			last_time            = std::chrono::system_clock::time_point::min();
		}

	public:
		char buf[chat_buffer_size] = "";
		bool overflow              = false;

		void flush_buffer()
		{
			if (length_flushed != len)
			{
				std::ofstream log(g_file_manager.get_project_file("./chat.log").get_path(), std::ios::app);
				std::string str(buf + length_flushed, buf + len);

				log << str;
				log.close();

				length_flushed = len;
			}

			if (overflow)
				reset_buf();
		}

		void append_msg(const char* player_name, char* msg)
		{
			char new_msg[msg_size] = "";

			int time_diff;
			auto currentTime = std::chrono::system_clock::now();

			if (last_time == std::chrono::system_clock::time_point::min())
				time_diff = 0;
			else
			{
				time_diff = std::chrono::duration_cast<std::chrono::seconds>(currentTime - last_time).count();
				if (time_diff > 99)
					time_diff = 0;
			}
			last_time = currentTime;

			std::string formatted_str = std::format("{} {} : {}\n", time_diff, player_name, msg);

			strcpy_safe(new_msg, formatted_str.c_str(), msg_size);

			auto msg_len          = strlen(new_msg);
			size_t availableSpace = chat_buffer_content_size - len;

			if (availableSpace < msg_len)
			{
				LOG(VERBOSE) << "Chat buffer overflow!";
				overflow = true;
			}

			len += msg_len;
			strcat(buf, new_msg);
		}
	};

	inline custom_chat_buffer g_custom_chat_buffer;
}