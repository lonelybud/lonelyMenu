#include "core/data/weapons.hpp"
#include "gta/weapons.hpp"
#include "natives.hpp"
#include "rage/joaat.hpp"
#include "services/gta_data/gta_data_service.hpp"
#include "services/gta_data/weapon_item.hpp"
#include "util/ped.hpp"
#include "util/strings.hpp"
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
			WEAPON::GIVE_DELAYED_WEAPON_TO_PED(ped::get_self_ped(), GADGET_PARACHUTE, 0, true);
		});
	}

	void view::weapons()
	{
		components::command_button<"fillammo">();

		ImGui::SeparatorText("Misc");

		render_misc();
	}
}
