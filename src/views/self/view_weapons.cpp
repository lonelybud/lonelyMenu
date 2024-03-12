#include "core/data/weapons.hpp"
#include "gta/weapons.hpp"
#include "natives.hpp"
#include "services/gta_data/gta_data_service.hpp"
#include "views/view.hpp"

namespace big
{
	static inline std::map<std::string, big::weapon_item> filter_weapons_map(const std::map<std::string, big::weapon_item>& inputMap, const std::string& search)
	{
		std::map<std::string, big::weapon_item> res;

		for (auto pair : inputMap)
		{
			auto t = pair.second.m_display_name;
			std::transform(t.begin(), t.end(), t.begin(), ::tolower);

			if (t.find(search) != std::string::npos)
				res[pair.first] = pair.second;
		}
		
		return res;
	}

	static inline void render_misc()
	{
		ImGui::BeginDisabled(g_weapons.aimbot.enable_old);
		components::command_checkbox<"aimbot">();
		ImGui::EndDisabled();
		ImGui::SameLine();
		ImGui::BeginDisabled(g_weapons.aimbot.enable);
		components::command_checkbox<"aimbot_old">();
		ImGui::EndDisabled();
		if (g_weapons.aimbot.enable || g_weapons.aimbot.enable_old)
		{
			ImGui::SetNextItemWidth(350);
			ImGui::SliderFloat("Aimbot Distance", &g_weapons.aimbot.distance, 1.f, 1000.f, "%.0f");
			ImGui::PushItemWidth(350);
			ImGui::SliderFloat("Aimbot Mid. Scr. Dist", &g_weapons.aimbot.max_dist_to_mid_of_scrn, 0.f, 1.f, "%.05f");
			ImGui::Checkbox("Aimbot Player", &g_weapons.aimbot.player);
		}

		ImGui::Spacing();

		components::button("Give Parachute", [] {
			WEAPON::GIVE_DELAYED_WEAPON_TO_PED(self::ped, GADGET_PARACHUTE, 0, true);
		});
		ImGui::SameLine();
		components::button("Remove Current Weapon", [] {
			Hash weaponHash;
			WEAPON::GET_CURRENT_PED_WEAPON(self::ped, &weaponHash, 1);
			if (weaponHash != "WEAPON_UNARMED"_J)
				WEAPON::REMOVE_WEAPON_FROM_PED(self::ped, weaponHash);
		});
	}

	static inline void render_ammunation()
	{
		if (ImGui::CollapsingHeader("Ammunation"))
		{
			static Hash selected_weapon_hash;
			static std::string selected_weapon, search_weapon_name;
			static std::map<std::string, big::weapon_item> searched_weapons;

			ImGui::SetNextItemWidth(300);
			if (components::input_text_with_hint("###search_weapon_name", "search name", search_weapon_name))
			{
				if (search_weapon_name.length() > 0)
				{
					std::transform(search_weapon_name.begin(), search_weapon_name.end(), search_weapon_name.begin(), ::tolower);
					searched_weapons = filter_weapons_map(g_gta_data_service->weapons(), search_weapon_name);
				}
				else
					searched_weapons.clear();
			}

			if (ImGui::BeginListBox("###weaponsList", {300, 100}))
			{
				for (auto& weapon : (search_weapon_name.length() > 0 ? searched_weapons : g_gta_data_service->weapons()))
					if (weapon.second.m_display_name != "NULL"
					    && ImGui::Selectable(weapon.second.m_display_name.c_str(), weapon.second.m_hash == selected_weapon_hash))
					{
						selected_weapon      = weapon.second.m_display_name;
						selected_weapon_hash = weapon.second.m_hash;
					}

				ImGui::EndListBox();
			}
			ImGui::SameLine();
			components::button("Give Weapon", [] {
				WEAPON::GIVE_WEAPON_TO_PED(self::ped, selected_weapon_hash, 9999, FALSE, TRUE);
			});
		}
	}

	void view::weapons()
	{
		components::button("Fill Ammo", [] {
			for (const auto& [_, weapon] : g_gta_data_service->weapons())
			{
				int ammo_in;
				WEAPON::GET_MAX_AMMO(self::ped, weapon.m_hash, &ammo_in);
				WEAPON::SET_PED_AMMO(self::ped, weapon.m_hash, ammo_in, 0);
			}
		});

		ImGui::SeparatorText("Misc");

		render_misc();

		ImGui::Spacing();

		render_ammunation();

		ImGui::Spacing();

		components::command_checkbox<"modify_weapon_values">();
		if (g_weapons.modify_weapon_values.enable)
		{
			ImGui::PushItemWidth(200);
			ImGui::InputFloat("Time Between Shots", &g_weapons.modify_weapon_values.time_bw_shots);
		}

		components::command_checkbox<"allow_all_weapons">();
		components::command_checkbox<"infclip">();
		components::command_checkbox<"rapidfire">();
		if (g_weapons.rapid_fire)
		{
			ImGui::PushItemWidth(200);
			ImGui::InputInt("Rapid Fire Delay", &g_weapons.rapid_fire_delay);
		}
	}
}
