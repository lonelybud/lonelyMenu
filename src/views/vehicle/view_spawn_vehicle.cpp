#include "core/data/vehicle.hpp"
#include "fiber_pool.hpp"
#include "natives.hpp"
#include "services/gta_data/gta_data_service.hpp"
#include "services/notifications/notification_service.hpp"
#include "util/blip.hpp"
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
		static bool spawn_maxed;
		static int selected_class = -1;
		string_vec& class_arr     = g_gta_data_service->vehicle_classes();
		static std::string search_veh_name;
		static vehicle_map searched_vehicles;

		ImGui::Checkbox("Spawn Maxed", &spawn_maxed);

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

		if (ImGui::BeginListBox("###vehicles", {300, 300}))
		{
			vehicle_map* temp_objs = nullptr;

			if (search_veh_name.length() > 0 || selected_class != -1)
			{
				if (!searched_vehicles.size())
					searched_vehicles = filter_vehicles(search_veh_name, selected_class);

				temp_objs = &searched_vehicles;
			}

			for (auto& pair : (temp_objs != nullptr ? *temp_objs : g_gta_data_service->vehicles()))
			{
				const auto& vehicle = pair.second;

				ImGui::PushID(vehicle.m_hash);
				components::selectable(vehicle.m_display_name, false, [&vehicle] {
					Vector3 spawn_location;
					std::optional<Vector3> waypoint_location;

					if (g_vehicle.spawn_at_waypoint)
					{
						if (auto waypoint_location = blip::get_waypoint_location(); waypoint_location.has_value())
							spawn_location = waypoint_location.value();
						else
							return;
					}
					else
						spawn_location = vehicle::get_spawn_location(vehicle.m_hash);

					auto veh  = vehicle::spawn(vehicle.m_hash, spawn_location);
					auto name = vehicle::get_vehicle_model_name(vehicle);

					if (veh == 0)
						g_notification_service->push_error("Spawn Vehicle", std::format("Unable to spawn {}", name), true);
					else
					{
						g_notification_service->push_success("Spawn Vehicle", std::format("Spawned {}", name), true);

						if (spawn_maxed)
							vehicle::max_vehicle(veh);
					}

					ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&veh);
				});
				ImGui::PopID();
			}
			ImGui::EndListBox();
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

		ImGui::Spacing();

		switch (spawn_vehicle_type)
		{
		case 0: render_spawn_new_vehicle(); break;
		case 1: view::persist_car(); break;
		}
	}
}
