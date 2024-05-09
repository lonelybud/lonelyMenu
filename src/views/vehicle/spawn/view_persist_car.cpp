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
	inline void save_vehicle_button(char* vehicle_file_name_input, const char* save_folder)
	{
		components::button("Save Vehicle", [vehicle_file_name_input, save_folder] {
			std::string yo = vehicle_file_name_input;
			
			if (!trimString(yo).size())
				return g_notification_service.push_warning("Persist Car", "Filename empty!");

			if (!self::veh)
				return g_notification_service.push_warning("Persist Car", "You must be in a vehicle. Please enter a vehicle before using load.");

			if (ENTITY::DOES_ENTITY_EXIST(self::veh))
			{
				yo += ".json";
				replace_double_dots(yo); // so that .. does not throw error by custom file system
				persist_car_service::save_vehicle(self::veh, yo, save_folder);
				ZeroMemory(vehicle_file_name_input, sizeof(vehicle_file_name_input));

				persist_car_service::refresh_list();
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
		static std::string file_name_to_delete{};

		if (!file_name_to_delete.empty())
			ImGui::OpenPopup("##deletepersistcar");

		if (ImGui::BeginPopupModal("##deletepersistcar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove))
		{
			ImGui::Text("Are you sure you want to delete %s?", file_name_to_delete.c_str());

			ImGui::Spacing();

			if (ImGui::Button("Yes"))
			{
				persist_car_service::delete_vehicle(file_name_to_delete);
				file_name_to_delete.clear();
				persist_car_service::refresh_list();
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
			persist_car_service::refresh_list();
		});

		ImGui::SetNextItemWidth(300.f);
		auto folder_display = g_vehicle.persist_vehicle.folder.empty() ? "Root" : g_vehicle.persist_vehicle.folder.c_str();
		if (ImGui::BeginCombo("Folder", folder_display))
		{
			if (ImGui::Selectable("Root", g_vehicle.persist_vehicle.folder == ""))
			{
				g_vehicle.persist_vehicle.folder.clear();
				persist_car_service::refresh_list();
			}

			for (std::string folder_name : g_vehicle.persist_vehicle.folders)
			{
				if (ImGui::Selectable(folder_name.c_str(), g_vehicle.persist_vehicle.folder == folder_name))
				{
					g_vehicle.persist_vehicle.folder = folder_name;
					persist_car_service::refresh_list();
				}
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
		const auto box_height = g_vehicle.persist_vehicle.files.size() <= 30 ? (g_vehicle.persist_vehicle.files.size() * ImGui::GetTextLineHeightWithSpacing() + 2) : over_30;
		if (ImGui::BeginListBox("##empty", ImVec2(300, box_height)))
		{
			for (const auto& pair : g_vehicle.persist_vehicle.files)
			{
				std::string pair_lower = pair;
				std::transform(pair_lower.begin(), pair_lower.end(), pair_lower.begin(), tolower);
				if (pair_lower.contains(lower_search))
				{
					auto file_name = pair.c_str();
					if (ImGui::Selectable(file_name, g_vehicle.persist_vehicle.file == pair, ImGuiSelectableFlags_AllowItemOverlap))
					{
						g_vehicle.persist_vehicle.file = pair;
						open_modal                     = true;
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
		static char vehicle_file_name_input[64]{};

		components::small_text("Vehicle File Name");
		ImGui::SetNextItemWidth(250);
		ImGui::InputText("##vehiclefilename", vehicle_file_name_input, IM_ARRAYSIZE(vehicle_file_name_input));
		if (ImGui::IsItemActive())
			g_hud.typing = TYPING_TICKS;
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Ex: My Cool Car");

		if (g_vehicle.persist_vehicle.folder.empty())
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
			save_vehicle_button(vehicle_file_name_input, g_vehicle.persist_vehicle.folder.c_str());

		ImGui::EndGroup();

		if (open_modal)
			ImGui::OpenPopup("##spawncarmodel2");
		if (ImGui::BeginPopupModal("##spawncarmodel2", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove))
		{
			ImGui::Text("Are you sure you want to spawn %s", g_vehicle.persist_vehicle.file.c_str());
			ImGui::Spacing();
			if (ImGui::Button("Yes"))
			{
				g_fiber_pool->queue_job([] {
					std::optional<Vector3> waypoint_location;
					if (g_vehicle.spawn_at_waypoint)
						if (waypoint_location = blip::get_blip_location((int)BlipIcons::Waypoint, -1, true);
						    !waypoint_location.has_value())
							return;

					persist_car_service::load_vehicle(waypoint_location, self::ped);
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
