#include "core/data/language_codes.hpp"
#include "core/scr_globals.hpp"
#include "services/players/player_service.hpp"
#include "views/view.hpp"

#include <script/globals/GPBD_FM_3.hpp>

namespace big
{
	void view::players_lang()
	{
		static int selected_index = 0;

		ImGui::SetNextItemWidth(200.f);
		if (ImGui::BeginCombo("##languages", languages[selected_index].name))
		{
			for (const auto& lang : languages)
				if (components::selectable(lang.name, selected_index == lang.id))
					selected_index = lang.id;

			ImGui::EndCombo();
		}

		ImGui::Spacing();

		if (ImGui::BeginListBox("##players", ImVec2(600, 400)))
		{
			for (const auto& [_, player] : g_player_service->players())
				if (player->is_valid() && scr_globals::gpbd_fm_3.as<GPBD_FM_3*>()->Entries[player->id()].BossGoon.Language == selected_index)
					ImGui::Text(player->m_name);

			ImGui::EndListBox();
		}
	}
}