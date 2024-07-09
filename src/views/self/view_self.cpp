#include "core/data/context_menu.hpp"
#include "core/data/self.hpp"
#include "natives.hpp"
#include "services/notifications/notification_service.hpp"
#include "util/local_player.hpp"
#include "views/view.hpp"

namespace big
{
	static inline void render_first_block()
	{
		components::button("Clear Wanted Level", [] {
			local_player::remove_wanted();
		});
		ImGui::SameLine();
		components::button("Heal", [] {
			ENTITY::SET_ENTITY_HEALTH(self::ped, PED::GET_PED_MAX_HEALTH(self::ped), 0, 0);
			PED::SET_PED_ARMOUR(self::ped, PLAYER::GET_PLAYER_MAX_ARMOUR(self::id));
		});
		ImGui::SameLine();
		components::button("Clean", [] {
			auto ped = self::ped;

			// https://forum.cfx.re/t/information-needed-to-clear-visible-player-damage-scars-etc/283216
			// https://docs.fivem.net/natives/?_0x397C38AA7B4A5F83
			PED::CLEAR_PED_DAMAGE_DECAL_BY_ZONE(ped, 0, "ALL");
			PED::CLEAR_PED_DAMAGE_DECAL_BY_ZONE(ped, 1, "ALL");
			PED::CLEAR_PED_DAMAGE_DECAL_BY_ZONE(ped, 2, "ALL");
			PED::CLEAR_PED_DAMAGE_DECAL_BY_ZONE(ped, 3, "ALL");
			PED::CLEAR_PED_DAMAGE_DECAL_BY_ZONE(ped, 4, "ALL");
			PED::CLEAR_PED_DAMAGE_DECAL_BY_ZONE(ped, 5, "ALL");

			PED::CLEAR_PED_BLOOD_DAMAGE(ped);
			PED::CLEAR_PED_ENV_DIRT(ped);
			PED::CLEAR_PED_WETNESS(ped);
			PED::CLEAR_PED_LAST_DAMAGE_BONE(ped);
			NETWORK::NETWORK_FORCE_LOCAL_PLAYER_SCAR_SYNC();
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
		ImGui::SameLine();
		components::button("Die", [] {
			ENTITY::SET_ENTITY_HEALTH(self::ped, 0, 0, 0);
		});

		ImGui::Spacing();

		components::button("Toggle Max Wanted Level", [] {
			auto is_zero = PLAYER::GET_MAX_WANTED_LEVEL() == 0;

			if (is_zero)
				PLAYER::SET_MAX_WANTED_LEVEL(5);
			else
				PLAYER::SET_MAX_WANTED_LEVEL(0);

			g_notification_service.push_success("Max Wanted Level", is_zero ? "Set to 5" : "Set to 0");
		});
		ImGui::SameLine();
		components::button("Ragdoll", [] {
			PED::SET_PED_TO_RAGDOLL(self::ped, 2000, 2000, 0, 0, 0, 0);
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
			components::command_checkbox<"never_wanted">();

			static bool is_ragdoll_disabled = false;
			if (ImGui::Checkbox("Disable Ragdoll", &is_ragdoll_disabled))
				g_fiber_pool->queue_job([] {
					PED::SET_PED_CAN_RAGDOLL(self::ped, !is_ragdoll_disabled);
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

		ImGui::SeparatorText("Context Menu###cxtmenu");

		ImGui::Checkbox("Enable", &g_context_menu.enabled);
		if (g_context_menu.enabled)
		{
			ImGui::Text("Allowed Entity Types:");
			ImGui::CheckboxFlags("Object", reinterpret_cast<int*>(&g_context_menu.allowed_entity_types), static_cast<int>(ContextEntityType::OBJECT));
			ImGui::SameLine();
			ImGui::CheckboxFlags("Ped", reinterpret_cast<int*>(&g_context_menu.allowed_entity_types), static_cast<int>(ContextEntityType::PED));
			ImGui::SameLine();
			ImGui::CheckboxFlags("Player", reinterpret_cast<int*>(&g_context_menu.allowed_entity_types), static_cast<int>(ContextEntityType::PLAYER));
			ImGui::SameLine();
			ImGui::CheckboxFlags("Vehicle", reinterpret_cast<int*>(&g_context_menu.allowed_entity_types), static_cast<int>(ContextEntityType::VEHICLE));
			ImGui::SameLine();
			ImGui::Checkbox("Dead", &g_context_menu.dead_entities);
		}
	}
}
