#include "services/custom_teleport/custom_teleport_service.hpp"
#include "util/teleport.hpp"
#include "views/view.hpp"

namespace big
{
	static std::vector<telelocation> telelocations;
	static std::string category = "";

	static void refresh_telelocations()
	{
		if (category.length())
			telelocations = g_custom_teleport_service.all_saved_locations[category];
	}

	void view::custom_teleport()
	{
		static std::string new_location_name{}, search;
		static const telelocation* selected_telelocation = nullptr;
		static bool delete_modal, selected;

		if (selected)
			ImGui::OpenPopup("##selectedlocation");
		else if (delete_modal)
			ImGui::OpenPopup("##deletelocation");

		if (ImGui::BeginPopupModal("##deletelocation", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove))
		{
			ImGui::Text("Are you sure you want to delete %s?", selected_telelocation->name);
			ImGui::Spacing();
			if (ImGui::Button("Yes"))
			{
				g_custom_teleport_service.delete_saved_location(category, selected_telelocation->name);
				selected_telelocation = nullptr;
				refresh_telelocations();

				delete_modal = false;
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

		if (ImGui::BeginPopupModal("##selectedlocation", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove))
		{
			ImGui::Text("Teleport to %s?", selected_telelocation->name);
			ImGui::Spacing();
			if (ImGui::Button("Yes"))
			{
				g_fiber_pool->queue_job([] {
					teleport::to_coords({selected_telelocation->x, selected_telelocation->y, selected_telelocation->z});
				});
				g_log.log_additional(std::format("Custom Teleport: {}, {}, {}, {}",
				    selected_telelocation->name,
				    selected_telelocation->x,
				    selected_telelocation->y,
				    selected_telelocation->z));

				selected = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("No"))
			{
				selected = false;
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
				refresh_telelocations();
			}
		});
		ImGui::SameLine();
		components::button("Refresh List", [] {
			g_custom_teleport_service.fetch_saved_locations();
			refresh_telelocations();
		});
		ImGui::SameLine();
		components::button("Delete Selected", [] {
			if (selected_telelocation)
				delete_modal = true;
		});

		ImGui::Separator();

		ImGui::Spacing();

		ImGui::SetNextItemWidth(300);
		if (components::input_text_with_hint("##search", "Search", search))
		{
			category = "";
			if (search.length() > 0)
				telelocations = g_custom_teleport_service.saved_telelocations_filtered_list(search);
			else
				telelocations.clear();
		}

		ImGui::BeginGroup();
		components::small_text("Categories");
		if (ImGui::BeginListBox("##categories", {250, static_cast<float>(*g_pointers->m_gta.m_resolution_y * 0.5)}))
		{
			if (!search.length())
				for (auto& l : g_custom_teleport_service.all_saved_locations | std::ranges::views::keys)
					if (ImGui::Selectable(l.c_str(), l == category))
					{
						category = l;
						refresh_telelocations();
					}
			ImGui::EndListBox();
		}
		ImGui::EndGroup();
		ImGui::SameLine();
		ImGui::BeginGroup();
		components::small_text("Locations");
		if (ImGui::BeginListBox("##telelocations", {250, static_cast<float>(*g_pointers->m_gta.m_resolution_y * 0.5)}))
		{
			for (const auto& l : telelocations)
				if (ImGui::Selectable(l.name.c_str(), selected_telelocation && selected_telelocation->name == l.name))
				{
					selected_telelocation = &l;
					selected              = true;
				}
			ImGui::EndListBox();
		}
		ImGui::EndGroup();
	}
}
