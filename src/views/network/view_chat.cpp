#include "core/data/session.hpp"
#include "gui.hpp"
#include "services/custom_chat_buffer.hpp"
#include "util/chat.hpp"
#include "views/view.hpp"

namespace big
{
	void view::chat()
	{
		static bool is_team;
		static char msg[256];

		ImGui::Checkbox("Log Messages (to textbox)", &g_session.log_chat_messages_to_textbox);

		ImGui::Spacing();

		ImGui::SetNextItemWidth(300);
		components::input_text_with_hint("##message", "Message", msg, sizeof(msg));
		ImGui::Checkbox("Is Team Message", &is_team);
		ImGui::SameLine();
		if (components::button("Send Message"))
		{
			g_gui->toggle(false); // trying to prevent game crash?
			g_fiber_pool->queue_job([] {
				chat::send_message(msg, nullptr, is_team);
				strcpy(msg, "");
			});
		};

		ImGui::Spacing();

		components::button("Reset Chat", [] {
			g_custom_chat_buffer.overflow = true;
		});
		ImGui::InputTextMultiline("##Text", g_custom_chat_buffer.buf, sizeof(g_custom_chat_buffer.buf), {600, 300}, ImGuiInputTextFlags_ReadOnly);
	}
}