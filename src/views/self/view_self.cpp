#include "core/scr_globals.hpp"
#include "gta_util.hpp"
#include "natives.hpp"
#include "util/pools.hpp"
#include "views/view.hpp"

#include <script/globals/GPBD_FM_3.hpp>

namespace big
{
	static inline void render_first_block()
	{
		components::command_button<"clearwantedlvl">();
		ImGui::SameLine();
		components::command_button<"heal">();
		ImGui::SameLine();
		components::button("Clean", [] {
			auto ped = self::ped;

			PED::CLEAR_PED_BLOOD_DAMAGE(ped);
			PED::CLEAR_PED_ENV_DIRT(ped);

			// https://forum.cfx.re/t/information-needed-to-clear-visible-player-damage-scars-etc/283216
			// https://docs.fivem.net/natives/?_0x397C38AA7B4A5F83
			PED::CLEAR_PED_DAMAGE_DECAL_BY_ZONE(ped, 0, "ALL");
			PED::CLEAR_PED_DAMAGE_DECAL_BY_ZONE(ped, 1, "ALL");
			PED::CLEAR_PED_DAMAGE_DECAL_BY_ZONE(ped, 2, "ALL");
			PED::CLEAR_PED_DAMAGE_DECAL_BY_ZONE(ped, 3, "ALL");
			PED::CLEAR_PED_DAMAGE_DECAL_BY_ZONE(ped, 4, "ALL");
			PED::CLEAR_PED_DAMAGE_DECAL_BY_ZONE(ped, 5, "ALL");
		});
	}

	static inline void render_chkboxs()
	{
		ImGui::BeginGroup();
		{
			components::command_checkbox<"infoxy">();
			components::command_checkbox<"godmode">();
		}
		ImGui::EndGroup();
	}

	static inline void render_other_options1()
	{
		components::command_button<"fillsnacks">();
	}

	static inline void render_other_options2()
	{
		components::button("Clear tasks", [] {
			TASK::CLEAR_PED_TASKS(self::ped);
		});
		ImGui::SameLine();
		components::button("Clear Attachments", [] {
			for (auto obj : pools::get_all_props())
			{
				auto object = g_pointers->m_gta.m_ptr_to_handle(obj);
				if (!object)
					break;

				if (!ENTITY::IS_ENTITY_ATTACHED_TO_ENTITY(self::ped, object))
					continue;

				ENTITY::DELETE_ENTITY(&object);
			}
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
