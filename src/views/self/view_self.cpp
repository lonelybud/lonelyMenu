#include "core/scr_globals.hpp"
#include "gta_util.hpp"
#include "natives.hpp"
#include "util/ped.hpp"
#include "views/view.hpp"

#include <script/globals/GPBD_FM_3.hpp>

namespace big
{
	static inline void render_first_block()
	{
		components::command_button<"clearwantedlvl">();
		ImGui::SameLine();
		components::command_button<"heal">();
	}

	static inline void render_chkboxs()
	{
		ImGui::BeginGroup();
		{
			components::command_checkbox<"infoxy">();
		}
		ImGui::EndGroup();
		ImGui::SameLine();
		ImGui::BeginGroup();
		{
			components::command_checkbox<"cleanloop">();
		}
		ImGui::EndGroup();
	}

	static inline void render_other_options1()
	{
		components::command_button<"skipcutscene">();
		ImGui::SameLine();
		components::command_button<"fillsnacks">();
	}

	static inline void render_other_options2()
	{
		components::button("Clear tasks", [] {
			TASK::CLEAR_PED_TASKS(ped::get_self_ped());
		});
	}

	void view::self()
	{
		ImGui::Spacing();

		render_first_block();

		ImGui::SeparatorText("###self_checkboxs");

		render_chkboxs();

		ImGui::Spacing();

		ImGui::SeparatorText("###otherOptions1");

		render_other_options1();

		ImGui::SeparatorText("###otherOptions2");

		render_other_options2();
	}
}
