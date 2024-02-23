#include "core/data/reactions.hpp"
#include "views/view.hpp"

namespace big
{
	void draw_reaction(reaction& reaction)
	{
		ImGui::PushID(&reaction);
		ImGui::Text(reaction.m_event_name);
		components::hor_space();
		ImGui::Checkbox("Notify", &reaction.notify);
		ImGui::SameLine();
		ImGui::Checkbox("Log", &reaction.log);
		ImGui::PopID();
	}

	void view::reaction_settings()
	{
		components::title("Reactions");

		components::sub_title("Player protection related");

		static std::string event_name;
		ImGui::SetNextItemWidth(200);
		if (components::input_text_with_hint("###event_name", "event name", event_name))
			std::transform(event_name.begin(), event_name.end(), event_name.begin(), ::tolower);

		for (reaction* i = &g_reactions.end_session_kick; i <= &g_reactions.give_weapon; ++i)
			if (event_name.length())
			{
				std::string t = i->m_event_name;
				std::transform(t.begin(), t.end(), t.begin(), ::tolower);

				if (t.find(event_name) != std::string::npos)
					draw_reaction(*i);
			}
			else
				draw_reaction(*i);
	}
}
