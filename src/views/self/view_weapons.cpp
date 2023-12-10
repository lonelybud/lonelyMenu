#include "core/data/weapons.hpp"
#include "gta/weapons.hpp"
#include "natives.hpp"
#include "services/gta_data/gta_data_service.hpp"
#include "views/view.hpp"

namespace big
{
	static inline void render_misc()
	{
		components::command_checkbox<"aimbot">();
		ImGui::SetNextItemWidth(350);
		ImGui::SliderFloat("Aimbot Distance", &g_weapons.aimbot.distance, 1.f, 1000.f, "%.0f");
		ImGui::Checkbox("Aimbot Player", &g_weapons.aimbot.player);

		ImGui::Spacing();

		components::button("Give Parachute", [] {
			WEAPON::GIVE_DELAYED_WEAPON_TO_PED(self::ped, GADGET_PARACHUTE, 0, true);
		});
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
	}
}
