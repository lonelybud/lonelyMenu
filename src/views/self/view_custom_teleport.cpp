#include "services/custom_teleport/custom_teleport_service.hpp"
#include "util/teleport.hpp"
#include "views/view.hpp"

namespace big
{
	void view::custom_teleport()
	{
		static std::string new_location_name{}, category, filter;
		static const telelocation* selected_telelocation = nullptr;
		static bool delete_modal;
		static std::vector<telelocation> search_results;

		if (delete_modal)
			ImGui::OpenPopup("##deletelocation");

		if (ImGui::BeginPopupModal("##deletelocation", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove))
		{
			ImGui::Text("Are you sure you want to delete %s?", selected_telelocation->name);
			ImGui::Spacing();
			if (ImGui::Button("Yes"))
			{
				g_custom_teleport_service.delete_saved_location(category, selected_telelocation->name);
				selected_telelocation = nullptr;
				delete_modal          = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("No"))
			{
				delete_modal = false;
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
				g_notification_service.push_warning("Custom Teleport", "category or name is empty");
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
		components::button("Teleport", [] {
			if (selected_telelocation)
			{
				g_log.log_additional(std::format("Custom Teleport: {}, {}, {}, {}",
				    selected_telelocation->name,
				    selected_telelocation->x,
				    selected_telelocation->y,
				    selected_telelocation->z));
				teleport::to_coords({selected_telelocation->x, selected_telelocation->y, selected_telelocation->z});
			}
		});
		ImGui::SameLine();
		components::button("Delete", [] {
			if (selected_telelocation)
				delete_modal = true;
		});

		ImGui::Spacing();

		ImGui::SetNextItemWidth(300);
		if (components::input_text_with_hint("##filter", "Search", filter))
		{
			if (filter.length() > 0)
				search_results = g_custom_teleport_service.saved_telelocations_filtered_list(filter);
			else
				search_results.clear();
		}

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
			auto& objs = filter.length() > 0 ? search_results :
			                                   (g_custom_teleport_service.all_saved_locations.find(category)
			                                               != g_custom_teleport_service.all_saved_locations.end() ?
			                                           g_custom_teleport_service.all_saved_locations[category] :
			                                           search_results);
			for (const auto& l : objs)
				if (ImGui::Selectable(l.name.c_str(), selected_telelocation && selected_telelocation->name == l.name))
					selected_telelocation = &l;
			ImGui::EndListBox();
		}
		ImGui::EndGroup();
	}
}
