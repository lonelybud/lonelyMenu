#include "core/data/context_menu.hpp"
#include "natives.hpp"
#include "util/local_player.hpp"
#include "views/view.hpp"

namespace big
{
	static inline void render_first_block()
	{
		components::button("Clear Wanted Level", [] {
			g_local_player->m_player_info->m_wanted_level = 0;
			g_local_player->m_player_info->m_is_wanted    = false;
		});
		ImGui::SameLine();
		components::button("Heal", [] {
			ENTITY::SET_ENTITY_HEALTH(self::ped, PED::GET_PED_MAX_HEALTH(self::ped), 0, 0);
			PED::SET_PED_ARMOUR(self::ped, PLAYER::GET_PLAYER_MAX_ARMOUR(self::id));
		});
		ImGui::SameLine();
		components::button("Clean", [] {
			auto ped = self::ped;

			PED::CLEAR_PED_BLOOD_DAMAGE(ped);
			PED::CLEAR_PED_ENV_DIRT(ped);
			PED::CLEAR_PED_WETNESS(ped);

			// https://forum.cfx.re/t/information-needed-to-clear-visible-player-damage-scars-etc/283216
			// https://docs.fivem.net/natives/?_0x397C38AA7B4A5F83
			PED::CLEAR_PED_DAMAGE_DECAL_BY_ZONE(ped, 0, "ALL");
			PED::CLEAR_PED_DAMAGE_DECAL_BY_ZONE(ped, 1, "ALL");
			PED::CLEAR_PED_DAMAGE_DECAL_BY_ZONE(ped, 2, "ALL");
			PED::CLEAR_PED_DAMAGE_DECAL_BY_ZONE(ped, 3, "ALL");
			PED::CLEAR_PED_DAMAGE_DECAL_BY_ZONE(ped, 4, "ALL");
			PED::CLEAR_PED_DAMAGE_DECAL_BY_ZONE(ped, 5, "ALL");
		});
		ImGui::SameLine();
		components::button("Fill Inventory", [] {
			std::string prefix = local_player::get_mp_prefix();
			STATS::STAT_SET_INT(rage::joaat(prefix + "NO_BOUGHT_YUM_SNACKS"), 30, true);
			STATS::STAT_SET_INT(rage::joaat(prefix + "NO_BOUGHT_HEALTH_SNACKS"), 15, true);
			STATS::STAT_SET_INT(rage::joaat(prefix + "NO_BOUGHT_EPIC_SNACKS"), 5, true);

			STATS::STAT_SET_INT(rage::joaat(prefix + "NUMBER_OF_ORANGE_BOUGHT"), 10, true);
			STATS::STAT_SET_INT(rage::joaat(prefix + "NUMBER_OF_BOURGE_BOUGHT"), 10, true);
			STATS::STAT_SET_INT(rage::joaat(prefix + "NUMBER_OF_SPRUNK_BOUGHT"), 10, true);

			STATS::STAT_SET_INT(rage::joaat(prefix + "NUMBER_OF_CHAMP_BOUGHT"), 5, true);

			STATS::STAT_SET_INT(rage::joaat(prefix + "CIGARETTES_BOUGHT"), 20, true);

			STATS::STAT_SET_INT(rage::joaat(prefix + "MP_CHAR_ARMOUR_1_COUNT"), 10, true);
			STATS::STAT_SET_INT(rage::joaat(prefix + "MP_CHAR_ARMOUR_2_COUNT"), 10, true);
			STATS::STAT_SET_INT(rage::joaat(prefix + "MP_CHAR_ARMOUR_3_COUNT"), 10, true);
			STATS::STAT_SET_INT(rage::joaat(prefix + "MP_CHAR_ARMOUR_4_COUNT"), 10, true);
			STATS::STAT_SET_INT(rage::joaat(prefix + "MP_CHAR_ARMOUR_5_COUNT"), 10, true);

			STATS::STAT_SET_INT(rage::joaat(prefix + "BREATHING_APPAR_BOUGHT"), 20, true);
		});
	}

	static inline void render_chkboxs()
	{
		ImGui::BeginGroup();
		{
			components::command_checkbox<"infoxy">();
			components::command_checkbox<"godmode">();
			components::command_checkbox<"otr">();
			components::command_checkbox<"noidlekick">();
			components::command_checkbox<"freecam">();
			ImGui::Checkbox("Context Menu", &g_context_menu.enabled);
			components::options_modal("Context Menu Modal", [] {
				ImGui::Text("Allowed Entity Types:");
				ImGui::CheckboxFlags("Object", reinterpret_cast<int*>(&g_context_menu.allowed_entity_types), static_cast<int>(ContextEntityType::OBJECT));
				ImGui::SameLine();
				ImGui::CheckboxFlags("Ped", reinterpret_cast<int*>(&g_context_menu.allowed_entity_types), static_cast<int>(ContextEntityType::PED));
				ImGui::SameLine();
				ImGui::CheckboxFlags("Player", reinterpret_cast<int*>(&g_context_menu.allowed_entity_types), static_cast<int>(ContextEntityType::PLAYER));
				ImGui::SameLine();
				ImGui::CheckboxFlags("Vehicle", reinterpret_cast<int*>(&g_context_menu.allowed_entity_types), static_cast<int>(ContextEntityType::VEHICLE));
				ImGui::Spacing();
				ImGui::Checkbox("Dead Entites", &g_context_menu.dead_entities);
			});

			static const char* ragdoll_button_text = "Disable Ragdoll";
			components::button(ragdoll_button_text, [] {
				if (PED::CAN_PED_RAGDOLL(self::ped))
				{
					PED::SET_PED_CAN_RAGDOLL(self::ped, FALSE);
					ragdoll_button_text = "Enable Ragdoll";
				}
				else
				{
					PED::SET_PED_CAN_RAGDOLL(self::ped, TRUE);
					ragdoll_button_text = "Disable Ragdoll";
				}
			});
		}
		ImGui::EndGroup();
	}

	void view::self()
	{
		ImGui::Spacing();

		render_first_block();

		ImGui::SeparatorText("###self_checkboxs");

		render_chkboxs();
	}
}
