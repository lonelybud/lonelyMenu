#include "core/data/reactions.hpp"
#include "core/settings/notifications.hpp"
#include "util/strings.hpp"
#include "views/view.hpp"

namespace big
{
	void draw_pair_option(const std::string_view name, decltype(g_notifications.player_join)& option)
	{
		ImGui::Text(name.data());

		ImGui::PushID(name.data());
		components::hor_space();
		ImGui::Checkbox("Notify", &option.notify);
		ImGui::SameLine();
		ImGui::Checkbox("Log", &option.log);
		ImGui::PopID();
	}

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
		components::input_text_with_hint("###event_name", "event name", event_name);

		for (reaction* i = &g_reactions.end_session_kick; i <= &g_reactions.give_weapon; ++i)
			if (event_name.length())
			{
				std::string lowercaseSearchString = toLowercase(event_name);
				if (std::string lowercaseStr = toLowercase(i->m_event_name); lowercaseStr.find(lowercaseSearchString) != std::string::npos)
					draw_reaction(*i);
			}
			else
				draw_reaction(*i);

		ImGui::Separator();

		components::sub_title("Network Player Manager");

		draw_pair_option("Player Join", g_notifications.player_join);
		draw_pair_option("Player Leave", g_notifications.player_leave);
	}
}
