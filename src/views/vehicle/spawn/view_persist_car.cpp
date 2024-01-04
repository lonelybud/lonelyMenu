#include "core/data/hud.hpp"
#include "core/data/vehicle.hpp"
#include "fiber_pool.hpp"
#include "services/vehicle/persist_car_service.hpp"
#include "util/blip.hpp"
#include "util/strings.hpp"
#include "util/teleport.hpp"
#include "util/vehicle.hpp"
#include "views/view.hpp"

namespace big
{
	static std::string persist_vehicle_sub_folder, selected_vehicle_file;

	static void load_vehicle()
	{
		if (!selected_vehicle_file.empty())
		{
			std::optional<Vector3> waypoint_location;
			if (g_vehicle.spawn_at_waypoint)
				waypoint_location = blip::get_waypoint_location();

			auto vehicle = persist_car_service::load_vehicle(selected_vehicle_file, persist_vehicle_sub_folder, waypoint_location);

			if (vehicle == 0)
				g_notification_service->push_error("Persist Car", std::format("Unable to spawn {}", selected_vehicle_file), true);
			else
			{
				g_notification_service->push_success("Persist Car", std::format("Spawned {}", selected_vehicle_file), true);
				g_vehicle.spawned_vehicles[vehicle] = {selected_vehicle_file};
				// ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&vehicle);
			}

			selected_vehicle_file.clear();
		}
		else
			g_notification_service->push_warning("Persist Car", "Select a file first");
	}

	inline void save_vehicle_button(char* vehicle_file_name_input, const char* save_folder)
	{
		components::button("Save Vehicle", [vehicle_file_name_input, save_folder] {
			if (!trimString(vehicle_file_name_input).size())
				return g_notification_service->push_warning("Persist Car", "Filename empty!");

			if (!self::veh)
				return g_notification_service->push_warning("Persist Car", "You must be in a vehicle. Please enter a vehicle before using load.");

			if (ENTITY::DOES_ENTITY_EXIST(self::veh))
			{
				const auto vehicle_file_name = std::string(vehicle_file_name_input).append(".json");
				persist_car_service::save_vehicle(self::veh, vehicle_file_name, save_folder);
				ZeroMemory(vehicle_file_name_input, sizeof(vehicle_file_name_input));
			}
		});
		ImGui::SameLine();
		components::button("Populate Name", [vehicle_file_name_input] {
			if (self::veh)
			{
				std::string name = vehicle::get_vehicle_model_name(self::veh);
				strcpy(vehicle_file_name_input, name.c_str());
			}
		});
	}

	void view::persist_car()
	{
		static std::vector<std::string> vehicle_folders, vehicle_files;
		static std::string file_name_to_delete{};

		if (!file_name_to_delete.empty())
			ImGui::OpenPopup("##deletepersistcar");

		if (ImGui::BeginPopupModal("##deletepersistcar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
		{
			ImGui::Text("Are you sure you want to delete %s?", file_name_to_delete.c_str());

			ImGui::Spacing();

			if (ImGui::Button("Yes"))
			{
				persist_car_service::delete_vehicle(file_name_to_delete, persist_vehicle_sub_folder);
				file_name_to_delete.clear();
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("No"))
			{
				file_name_to_delete.clear();
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		components::button("Refresh List", [] {
			vehicle_folders = persist_car_service::list_sub_folders();
			vehicle_files   = persist_car_service::list_files(persist_vehicle_sub_folder);
		});

		ImGui::SetNextItemWidth(300.f);
		auto folder_display = persist_vehicle_sub_folder.empty() ? "Root" : persist_vehicle_sub_folder.c_str();
		if (ImGui::BeginCombo("Folder", folder_display))
		{
			if (ImGui::Selectable("Root", persist_vehicle_sub_folder == ""))
				persist_vehicle_sub_folder.clear();

			for (std::string folder_name : vehicle_folders)
			{
				if (ImGui::Selectable(folder_name.c_str(), persist_vehicle_sub_folder == folder_name))
					persist_vehicle_sub_folder = folder_name;
			}

			ImGui::EndCombo();
		}

		static char search[64];
		ImGui::SetNextItemWidth(300.f);
		components::input_text_with_hint("File Name", "Search", search, sizeof(search), ImGuiInputTextFlags_None);
		std::string lower_search = search;
		std::transform(lower_search.begin(), lower_search.end(), lower_search.begin(), tolower);
		static bool open_modal = false;

		ImGui::SetNextItemWidth(250);
		ImGui::Text("Saved Vehicles");

		static const auto over_30 = (30 * ImGui::GetTextLineHeightWithSpacing() + 2);
		const auto box_height = vehicle_files.size() <= 30 ? (vehicle_files.size() * ImGui::GetTextLineHeightWithSpacing() + 2) : over_30;
		if (ImGui::BeginListBox("##empty", ImVec2(300, box_height)))
		{
			for (const auto& pair : vehicle_files)
			{
				std::string pair_lower = pair;
				std::transform(pair_lower.begin(), pair_lower.end(), pair_lower.begin(), tolower);
				if (pair_lower.contains(lower_search))
				{
					auto file_name = pair.c_str();
					if (ImGui::Selectable(file_name, selected_vehicle_file == pair, ImGuiSelectableFlags_AllowItemOverlap))
					{
						selected_vehicle_file = pair;
						open_modal            = true;
					}

					ImGui::SameLine();
					ImGui::PushID(file_name);
					if (ImGui::SmallButton("X"))
						file_name_to_delete = pair;
					ImGui::PopID();
				}
			}

			ImGui::EndListBox();
		}

		ImGui::SameLine();

		ImGui::BeginGroup();
		static char vehicle_file_name_input[50]{};

		components::small_text("Vehicle File Name");
		ImGui::SetNextItemWidth(250);
		ImGui::InputText("##vehiclefilename", vehicle_file_name_input, IM_ARRAYSIZE(vehicle_file_name_input));
		if (ImGui::IsItemActive())
			g_hud.typing = TYPING_TICKS;
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Ex: My Cool Car");

		if (persist_vehicle_sub_folder.empty())
		{
			static char save_folder[50]{};
			components::small_text("Vehicle Folder Name");
			ImGui::SetNextItemWidth(250);
			ImGui::InputText("##foldername", save_folder, IM_ARRAYSIZE(save_folder));
			if (ImGui::IsItemActive())
				g_hud.typing = TYPING_TICKS;
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Ex: My Cool Car Collection (Leave this blank for Root)");

			save_vehicle_button(vehicle_file_name_input, save_folder);
		}
		else
			save_vehicle_button(vehicle_file_name_input, persist_vehicle_sub_folder.c_str());

		ImGui::EndGroup();

		if (open_modal)
			ImGui::OpenPopup("##spawncarmodel2");
		if (ImGui::BeginPopupModal("##spawncarmodel2", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
		{
			ImGui::Text(std::format("Are you sure you want to spawn {}", selected_vehicle_file).c_str());
			ImGui::Spacing();
			if (ImGui::Button("Yes"))
			{
				g_fiber_pool->queue_job([] {
					load_vehicle();
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
}
