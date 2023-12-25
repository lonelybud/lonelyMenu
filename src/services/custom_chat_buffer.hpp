#pragma once
#include "file_manager.hpp"

constexpr int chat_buffer_size         = 4096;
constexpr int chat_buffer_content_size = chat_buffer_size - 1;

namespace big
{
	class custom_chat_buffer
	{
		int len = 0;

	public:
		char buf[chat_buffer_size] = {'\0'};

		inline void reset_buf()
		{
			strcpy(buf, "");
			len = 0;
		}

		void flush_buffer()
		{
			std::ofstream log(g_file_manager.get_project_file("./chat.log").get_path(), std::ios::app);
			log << buf << std::endl;
			log.close();

			reset_buf();
		}

		void append_msg(const char* player_name, char* msg)
		{
			char new_msg[320]         = {'\0'};
			std::string formatted_str = std::format("{} : {}\n", player_name, msg);
			strcpy(new_msg, formatted_str.c_str());

			auto msg_len          = strlen(new_msg);
			size_t availableSpace = chat_buffer_content_size - len;

			if (availableSpace < msg_len)
			{
				LOG(WARNING) << "Chat buffer overflow: Flushing buffer to disk..";
				flush_buffer();
			}

			len += msg_len;
			strcat(buf, new_msg);
		}
	};

	inline custom_chat_buffer g_custom_chat_buffer;
}