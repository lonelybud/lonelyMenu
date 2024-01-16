#include "services/custom_teleport/custom_teleport_service.hpp"
#include "util/teleport.hpp"
#include "views/view.hpp"

namespace big
{
	void view::custom_teleport()
	{
		static std::string new_location_name{};
		static std::string category = "Default";
		static telelocation deletion_telelocation;

		if (!std::string(deletion_telelocation.name).empty())
			ImGui::OpenPopup("##deletelocation");

		if (ImGui::BeginPopupModal("##deletelocation", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
		{
			ImGui::Text("Are you sure you want to delete %s?", deletion_telelocation.name);
			ImGui::Spacing();
			if (ImGui::Button("Yes"))
			{
				g_custom_teleport_service.delete_saved_location(category, deletion_telelocation.name);
				deletion_telelocation.name = "";
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("No"))
			{
				deletion_telelocation.name = "";
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		ImGui::PushItemWidth(300);
		components::input_text_with_hint("Category", "", category);
		components::input_text_with_hint("Location Name", "", new_location_name);
		ImGui::PopItemWidth();

		components::button("Save Current Location", [] {
			if (category.empty() || new_location_name.empty())
				g_notification_service->push_warning("Custom Teleport", "category or name is empty");
			else
			{
				auto coords = ENTITY::GET_ENTITY_COORDS(self::ped, 0);
				g_custom_teleport_service.save_new_location(category, {new_location_name, coords.x, coords.y, coords.z});
			}
		});
		ImGui::SameLine();
		components::button("Refresh List", [] {
			g_custom_teleport_service.fetch_saved_locations();
		});

		ImGui::Separator();

		components::small_text("Double click to teleport.");
		components::small_text("Shift click to delete.");

		ImGui::Spacing();

		ImGui::BeginGroup();
		components::small_text("Categories");
		if (ImGui::BeginListBox("##categories", {250, static_cast<float>(*g_pointers->m_gta.m_resolution_y * 0.5)}))
		{
			for (auto& l : g_custom_teleport_service.all_saved_locations | std::ranges::views::keys)
				if (ImGui::Selectable(l.c_str(), l == category))
					category = l;
			ImGui::EndListBox();
		}
		ImGui::EndGroup();
		ImGui::SameLine();
		ImGui::BeginGroup();
		components::small_text("Locations");
		if (ImGui::BeginListBox("##telelocations", {250, static_cast<float>(*g_pointers->m_gta.m_resolution_y * 0.5)}))
		{
			if (g_custom_teleport_service.all_saved_locations.find(category)
			    != g_custom_teleport_service.all_saved_locations.end())
			{
				for (const auto& l : g_custom_teleport_service.all_saved_locations.at(category))
					if (ImGui::Selectable(l.name.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick))
					{
						if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
							deletion_telelocation = l;
						else if (ImGui::IsMouseDoubleClicked(0))
							g_fiber_pool->queue_job([l] {
								teleport::to_coords({l.x, l.y, l.z});
							});
					}
			}
			ImGui::EndListBox();
		}
		ImGui::EndGroup();
	}
}
