#include "core/data/filter_player.hpp"
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
		ImGui::SameLine();
		bool is_filter_active = g_filter_players_reaction == &reaction;
		if (ImGui::Checkbox("Filter", &is_filter_active))
			g_filter_players_reaction = is_filter_active ? &reaction : nullptr;
		ImGui::PopID();
	}

	void view::reaction_settings()
	{
		components::title("Reactions");

		ImGui::Checkbox("Block Reactions", &g_block_reaction);

		ImGui::Spacing();

		if (components::button("Clear filter"))
			g_filter_players_reaction = nullptr;

		ImGui::Spacing();

		static std::string event_name;
		ImGui::SetNextItemWidth(200);
		if (components::input_text_with_hint("###event_name", "event name", event_name))
			std::transform(event_name.begin(), event_name.end(), event_name.begin(), ::tolower);

		auto start = reinterpret_cast<reaction*>(&g_reactions);
		auto end = reinterpret_cast<reaction*>(reinterpret_cast<std::byte*>(start) + sizeof(g_reactions));

		for (reaction* i = start; i < end; ++i)
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
