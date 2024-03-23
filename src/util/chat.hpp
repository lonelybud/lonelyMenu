#pragma once
#include "core/data/session.hpp"
#include "hooking/hooking.hpp"
#include "natives.hpp"
#include "packet.hpp"
#include "script.hpp"
#include "services/custom_chat_buffer.hpp"
#include "services/players/player_service.hpp"
#include "util/player.hpp"

#include <script/HudColor.hpp>

namespace big::chat
{
	static inline void draw_chat(const char* msg, const char* player_name, bool is_team)
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

	static inline void gamer_handle_serialize(rage::rlGamerHandle& hnd, rage::datBitBuffer& buf)
	{
		constexpr int PC_PLATFORM = 3;
		buf.Write<uint8_t>(PC_PLATFORM, 8);
		buf.WriteInt64(*(int64_t*)&hnd.m_rockstar_id, 64);
		buf.Write<uint8_t>(hnd.unk_0009, 8);
	}

	inline void send_message(char* message, player_ptr target = nullptr, bool is_team = false)
	{
		if (!*g_pointers->m_gta.m_is_session_started)
			return;

		packet msg{};
		msg.write_message(rage::eNetMessage::MsgTextMessage);
		msg.m_buffer.WriteString(message, 256);
		gamer_handle_serialize(g_player_service->get_self()->get_net_data()->m_gamer_handle, msg.m_buffer);
		msg.write<bool>(is_team, 1);

		for (auto& player : g_player_service->players())
			if (player.second && player.second->is_valid() && (!target || target == player.second)
			    && (!is_team || is_player_same_team(player.second->id())))
				msg.send(player.second->get_net_game_player()->m_msg_id);

		if (g_session.log_chat_messages_to_textbox)
			g_custom_chat_buffer.append_msg(g_player_service->get_self()->get_name(), message);

		g_fiber_pool->queue_job([message, is_team] {
			draw_chat(message, g_player_service->get_self()->get_name(), is_team);
		});
	}
}
