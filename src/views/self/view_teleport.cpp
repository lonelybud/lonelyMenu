#include "core/data/self.hpp"
#include "util/mobile.hpp"
#include "util/teleport.hpp"
#include "views/view.hpp"

namespace big
{
	void view::teleport()
	{
		ImGui::BeginGroup();
		{
			components::ver_space();
			components::ver_space();
			ImGui::PushItemWidth(200);
			ImGui::Text("Blips:");
			{
				ImGui::Spacing();

				components::button("Waypoint TP", [] {
					teleport::to_waypoint();
				});
				ImGui::SameLine();
				components::button("Objective TP", [] {
					teleport::to_objective();
				});
			}
			components::ver_space();
			components::ver_space();
			ImGui::Text("Relative Position");
			{
				ImGui::Spacing();

				static float relative_pos[3];

				components::small_text("X for L/R, Y for F/B, Z for U/D");
				ImGui::InputFloat3("###relativepos", relative_pos);

				components::button("Relative TP###relativetp", [] {
					auto location = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(self::ped, relative_pos[0], relative_pos[1], relative_pos[2]);
					teleport::to_coords(location);
				});
				ImGui::SameLine();
				components::button("Reset##relative", [] {
					relative_pos[0] = relative_pos[1] = relative_pos[2] = 0;
				});
			}
			components::ver_space();
			components::ver_space();
			ImGui::Text("Coordinates");
			{
				ImGui::Spacing();

				float coords[3] = {self::pos.x, self::pos.y, self::pos.z};

				ImGui::InputFloat3("##currentcoordinates", coords, "%f", ImGuiInputTextFlags_ReadOnly);

				components::button("Copy to custom", [coords] {
					std::copy(std::begin(coords), std::end(coords), std::begin(g_self.custom_coords));
				});
				ImGui::SameLine();
				components::button("Copy to Clipboard", [coords] {
					ImGui::SetClipboardText(
					    std::format("X: {:.2f}, Y: {:.2f}, Z: {:.2f}", coords[0], coords[1], coords[2]).c_str());
				});

				components::small_text("Coord##teleport");
				ImGui::InputFloat3("##Customlocation", g_self.custom_coords);

				components::button("Coord TP##customtp", [] {
					teleport::to_coords({g_self.custom_coords[0], g_self.custom_coords[1], g_self.custom_coords[2]});
				});
			}
			ImGui::PopItemWidth();
		}
		ImGui::EndGroup();
		components::hor_space();
		ImGui::BeginGroup();
		view::custom_teleport();
		ImGui::EndGroup();
	}
}
