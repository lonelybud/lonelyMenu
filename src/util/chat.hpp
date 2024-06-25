#pragma once
#include "core/data/session.hpp"
#include "gta_util.hpp"
#include "hooking/hooking.hpp"
#include "natives.hpp"
#include "script.hpp"
#include "services/players/player_service.hpp"
#include "strings.hpp"

#include <script/HudColor.hpp>

namespace big::chat
{
	inline void draw_chat(const char* msg, const char* player_name, bool is_team)
	{
		int scaleform = GRAPHICS::REQUEST_SCALEFORM_MOVIE("MULTIPLAYER_CHAT");

		while (!GRAPHICS::HAS_SCALEFORM_MOVIE_LOADED(scaleform))
			script::get_current()->yield();

		GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(scaleform, "ADD_MESSAGE");
		GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(player_name); // player name
		GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_LITERAL_STRING(msg);             // content
		GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_TEXTURE_NAME_STRING(HUD::GET_FILENAME_FOR_AUDIO_CONVERSATION(is_team ? "MP_CHAT_TEAM" : "MP_CHAT_ALL")); // scope
		GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_BOOL(false);                               // teamOnly
		GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT((int)HudColor::HUD_COLOUR_PURE_WHITE); // eHudColour
		GRAPHICS::END_SCALEFORM_MOVIE_METHOD();

		GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(scaleform, "SET_FOCUS");
		GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(1);                                    // VISIBLE_STATE_DEFAULT
		GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(0);                                    // scopeType (unused)
		GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(0);                                    // scope (unused)
		GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(player_name);           // player
		GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT((int)HudColor::HUD_COLOUR_PURE_WHITE); // eHudColour
		GRAPHICS::END_SCALEFORM_MOVIE_METHOD();

		GRAPHICS::DRAW_SCALEFORM_MOVIE_FULLSCREEN(scaleform, 255, 255, 255, 255, 0);

		HUD::CLOSE_MP_TEXT_CHAT();
	}

	inline void send_chat_message(char* message, bool is_team)
	{
		std::string _message = message;
		if (trimString(_message).size())
		{
			g_session.sending_chat_msg = true;

			auto duration_ms = 0;
			auto currentTime = std::chrono::system_clock::now();

			// if not sending the message first time
			if (g_player_service->get_self()->last_msg_time != std::chrono::system_clock::time_point::min())
			{
				auto diff_ms =
				    std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - g_player_service->get_self()->last_msg_time)
				        .count();

				auto genuine_time_ms = (strlen(message) / (float)5) * 1000; // (length in chars / (5 char / 1 sec)) * no of ms in one sec
				if (genuine_time_ms > diff_ms)
					duration_ms = genuine_time_ms - diff_ms + 200;
			}

			g_fiber_pool->queue_job([duration_ms, message, is_team] {
				script::get_current()->yield(std::chrono::milliseconds(duration_ms));

				if (g_pointers->m_gta.m_is_session_started && gta_util::get_network()->m_game_session_state > 4)
				{
					const auto net_game_player = gta_util::get_network_player_mgr()->m_local_net_player;
					g_hooking->get_original<hooks::send_chat_message>()(*g_pointers->m_gta.m_send_chat_ptr,
					    net_game_player->get_net_data(),
					    message,
					    is_team);
					draw_chat(message, g_player_service->get_self()->m_name, is_team);

					g_player_service->get_self()->last_msg_time = std::chrono::system_clock::now();
					g_session.sending_chat_msg                  = false;
				}
			});
		}
	}

	static bool is_spam_interval_diff_there(char* msg, big::player_ptr _player, std::chrono::milliseconds diff, int limit, int type)
	{
		if (_player->last_spam_interval_diff != std::chrono::milliseconds::min())
		{
			// the diff bw gaps of arrival of messages is less than 1 sec
			if (abs(_player->last_spam_interval_diff - diff).count() < 1000)
			{
				if (type == 1)
				{
					if (++_player->same_interval_spam_count_high == limit)
					{
						g_log.log_additional(std::format("Chat Spammer - p {}, i1 {}, i2 {}, t {}, c {}",
						    _player->m_name,
						    _player->last_spam_interval_diff,
						    diff,
						    1,
						    _player->same_interval_spam_count_high));
						return true;
					}
				}
				else
				{
					if (++_player->same_interval_spam_count_low == limit)
					{
						g_log.log_additional(std::format("Chat Spammer - p {}, i1 {}, i2 {}, t {}, c {}",
						    _player->m_name,
						    _player->last_spam_interval_diff,
						    diff,
						    0,
						    _player->same_interval_spam_count_low));
						return true;
					}
				}
			}
			else
			{
				_player->same_interval_spam_count_low  = 0;
				_player->same_interval_spam_count_high = 0;
			}
		}

		_player->last_spam_interval_diff = diff;
		return false;
	}

	inline bool is_player_spammer(char* msg, big::player_ptr _player)
	{
		auto currentTime = std::chrono::system_clock::now();

		// first message should be allowed
		if (_player->last_msg_time == std::chrono::system_clock::time_point::min())
		{
			_player->last_msg_time = currentTime;
			return false;
		}

		auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - _player->last_msg_time);
		_player->last_msg_time = currentTime;

		if (diff.count() == 0) // prevent EXCEPTION_INT_DIVIDE_BY_ZERO
			return true;

		auto msg_len   = strlen(msg);
		auto time_secs = diff.count() / 1000.f;

		/** 
		 * detection based on typing speed 
		 * 
		 * english typing speed = 7chars/1sec.
		 * chinese typing speed = 2chars/1sec.
		 *   Given 情色影院 length = 4 * (3*1) = 12 i.e UTF-8 encodes "超" as 3 bytes
		 *   gives ratio 12/2 = 6/1 < 7/1 = OK.
		 * should work for russian language too but still experimental for now
		 */
		bool typing_speed_ratio_detection = false;
		if ((msg_len / time_secs) > 7)
			typing_speed_ratio_detection = true;

		if (typing_speed_ratio_detection || msg_len > 50)
		{
			// // it should take atleast 2 seconds: to type or copy and paste
			// if (diff.count() <= 2)
			// 	return true;

			return is_spam_interval_diff_there(msg, _player, diff, 2, 1);
		}

		return is_spam_interval_diff_there(msg, _player, diff, 10, 0);
	}
}
