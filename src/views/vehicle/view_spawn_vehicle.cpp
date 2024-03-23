#include "core/data/vehicle.hpp"
#include "fiber_pool.hpp"
#include "natives.hpp"
#include "services/gta_data/gta_data_service.hpp"
#include "services/notifications/notification_service.hpp"
#include "util/blip.hpp"
#include "util/teleport.hpp"
#include "util/vehicle.hpp"
#include "views/view.hpp"

namespace big
{
	static inline vehicle_map filter_vehicles(const std::string& search, int selected_class)
	{
		vehicle_map result;
		string_vec& class_arr = g_gta_data_service->vehicle_classes();

		for (auto& pair : g_gta_data_service->vehicles())
		{
			const auto& vehicle = pair.second;

			std::string display_name         = vehicle.m_display_name;
			std::string display_manufacturer = vehicle.m_display_manufacturer;
			std::string clazz                = vehicle.m_vehicle_class;
			std::string lower_search         = search;

			std::transform(display_name.begin(), display_name.end(), display_name.begin(), ::tolower);
			std::transform(display_manufacturer.begin(), display_manufacturer.end(), display_manufacturer.begin(), ::tolower);
			std::transform(lower_search.begin(), lower_search.end(), lower_search.begin(), tolower);

			auto class_check = selected_class == -1 || class_arr[selected_class] == clazz;
			auto search_check =
			    !lower_search.length() || (display_name.find(lower_search) != std::string::npos || display_manufacturer.find(lower_search) != std::string::npos);

			if (class_check && search_check)
				result.emplace(pair);
		}

		return result;
	}

	void render_spawn_new_vehicle()
	{
		static int selected_class = -1;
		string_vec& class_arr     = g_gta_data_service->vehicle_classes();
		static std::string search_veh_name;
		static vehicle_map searched_vehicles;
		static vehicle_item selected_veh;
		static bool open_modal = false;

		ImGui::Spacing();

		ImGui::SetNextItemWidth(300.f);
		if (ImGui::BeginCombo("Vehicle Class", selected_class == -1 ? "All" : class_arr[selected_class].c_str()))
		{
			if (ImGui::Selectable("All", selected_class == -1))
			{
				selected_class = -1;
				searched_vehicles.clear();
			}

			for (int i = 0; i < class_arr.size(); i++)
			{
				if (ImGui::Selectable(class_arr[i].c_str(), selected_class == i))
				{
					selected_class = i;
					searched_vehicles.clear();
				}

				if (selected_class == i)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::SetNextItemWidth(300.f);
		if (components::input_text_with_hint("Model Name", "Search", search_veh_name))
			searched_vehicles.clear();

		if ((search_veh_name.length() > 0 || selected_class != -1) && !searched_vehicles.size())
			searched_vehicles = filter_vehicles(search_veh_name, selected_class);

		if (ImGui::BeginListBox("###vehicles", {300, 300}))
		{
			for (auto& pair :
			    ((search_veh_name.length() > 0 || selected_class != -1) ? searched_vehicles : g_gta_data_service->vehicles()))
			{
				const auto& vehicle = pair.second;

				ImGui::PushID(vehicle.m_hash);
				if (components::selectable(vehicle.m_display_name, false))
				{
					selected_veh = vehicle;
					open_modal   = true;
				}

				ImGui::PopID();
			}
			ImGui::EndListBox();
		}

		if (open_modal)
			ImGui::OpenPopup("##spawncarmodel");
		if (ImGui::BeginPopupModal("##spawncarmodel", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove))
		{
			ImGui::Text("Are you sure you want to spawn %s", selected_veh.m_display_name);
			ImGui::Spacing();
			if (ImGui::Button("Yes"))
			{
				g_fiber_pool->queue_job([] {
					Vector3 spawn_location;
					std::optional<Vector3> waypoint_location;

					if (g_vehicle.spawn_at_waypoint)
					{
						if (auto waypoint_location = blip::get_blip_location((int)BlipIcons::Waypoint, -1, true);
						    waypoint_location.has_value())
							spawn_location = waypoint_location.value();
						else
							return;
					}
					else if (auto loc = vehicle::get_spawn_location(selected_veh.m_hash, self::ped); loc.has_value())
						spawn_location = loc.value();
					else
						return;

					auto veh  = vehicle::spawn(selected_veh.m_hash, spawn_location);
					auto name = vehicle::get_vehicle_model_name(selected_veh);

					if (veh == -1)
						return;

					if (veh == 0)
						g_notification_service.push_error("Spawn Vehicle", std::format("Unable to spawn {}", name), true);
					else
					{
						vehicle::repair(veh);

						if (g_vehicle.spawn_inside)
							teleport::into_vehicle(veh);

						g_notification_service.push_success("Spawn Vehicle", std::format("Spawned {}", name), true);
					}
				});

				open_modal = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("No"))
			{
				open_modal = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	void view::spawn_vehicle()
	{
		static int spawn_vehicle_type;

		ImGui::RadioButton("New", &spawn_vehicle_type, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Persistent", &spawn_vehicle_type, 1);

		ImGui::Spacing();

		ImGui::Checkbox("Spawn at waypoint", &g_vehicle.spawn_at_waypoint);
		ImGui::SameLine();
		ImGui::Checkbox("Tp inside", &g_vehicle.spawn_inside);

		ImGui::Spacing();

		switch (spawn_vehicle_type)
		{
		case 0: render_spawn_new_vehicle(); break;
		case 1: view::persist_car(); break;
		}
	}
}
