#include "util/mobile.hpp"
#include "util/teleport.hpp"
#include "views/view.hpp"

namespace big
{
	void view::teleport()
	{
		ImGui::SeparatorText("Blips:");
		ImGui::Spacing();

		components::button("Waypoint", [] {
			teleport::to_waypoint();
		});
		ImGui::SameLine();
		components::button("Objective", [] {
			teleport::to_objective(false);
		});
		ImGui::SameLine();
		components::button("Tp to PV", [] {
			if (Vehicle veh = mobile::mechanic::get_personal_vehicle(); self::veh != veh)
				teleport::into_vehicle(veh);
		});
		ImGui::SeparatorText("Direction");

		ImGui::Spacing();

		static float relative_pos[3];
		components::small_text("X for L/R, Y for F/B, Z for U/D");
		ImGui::InputFloat3("Relative Position", relative_pos);
		ImGui::SameLine();
		components::button("TP###relative", [] {
			auto location = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(self::ped, relative_pos[0], relative_pos[1], relative_pos[2]);
			PED::SET_PED_COORDS_KEEP_VEHICLE(self::ped, location.x, location.y, location.z);
		});

		ImGui::SeparatorText("Coordinates");

		ImGui::Spacing();

		components::small_text("Current coordinates");
		float coords[3] = {self::pos.x, self::pos.y, self::pos.z};
		static float new_location[3];

		ImGui::SetNextItemWidth(400);
		ImGui::InputFloat3("##currentcoordinates", coords, "%f", ImGuiInputTextFlags_ReadOnly);

		components::button("Copy to custom", [coords] {
			std::copy(std::begin(coords), std::end(coords), std::begin(new_location));
		});
		ImGui::SameLine();
		components::button("Copy to Clipboard", [coords] {
			ImGui::SetClipboardText(std::format("X: {:.2f}, Y: {:.2f}, Z: {:.2f}", coords[0], coords[1], coords[2]).c_str());
		});

		components::small_text("Custom teleport");
		ImGui::SetNextItemWidth(400);
		ImGui::InputFloat3("##Customlocation", new_location);
		components::button("Teleport", [] {
			teleport::to_coords({new_location[0], new_location[1], new_location[2]});
		});
	}
}
