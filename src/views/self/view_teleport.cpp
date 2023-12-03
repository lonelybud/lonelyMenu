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

		ImGui::SeparatorText("Direction");

		ImGui::Spacing();

		static float increment = 1;
		ImGui::SetNextItemWidth(100);
		ImGui::InputFloat("Teleport forward distance", &increment);
		ImGui::SameLine();
		components::button("Tel. Horizontal", [] {
			auto location = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(self::ped, 0, increment, 0);
			PED::SET_PED_COORDS_KEEP_VEHICLE(self::ped, location.x, location.y, location.z);
		});
		ImGui::SameLine();
		components::button("Tel. Vertical", [] {
			auto location = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(self::ped, 0, 0, increment);
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
			teleport::to_coords({new_location[0], new_location[1], new_location[2]}, true);
		});
	}
}
