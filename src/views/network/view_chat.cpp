#include "core/data/session.hpp"
#include "services/custom_chat_buffer.hpp"
#include "util/chat.hpp"
#include "views/view.hpp"

#include <imgui.h>

namespace big
{
	void view::chat()
	{
		static bool is_team;
		static char msg[256];

		ImGui::Checkbox("Log Messages (to textbox)", &g_session.log_chat_messages_to_textbox);

		ImGui::Spacing();

		ImGui::BeginDisabled(g_session.sending_chat_msg);

		ImGui::SetNextItemWidth(300);
		components::input_text_with_hint("##message", "Message", msg, sizeof(msg));
		ImGui::Checkbox("Is Team Message", &is_team);
		ImGui::SameLine();
		if (components::button("Send Message"))
			chat::send_chat_message(msg, is_team);

		ImGui::EndDisabled();

		ImGui::Spacing();

		components::button("Reset Chat", [] {
			g_custom_chat_buffer.overflow = true;
		});
		ImGui::InputTextMultiline("##Text", g_custom_chat_buffer.buf, sizeof(g_custom_chat_buffer.buf), {600, 300}, ImGuiInputTextFlags_ReadOnly);
	}
}