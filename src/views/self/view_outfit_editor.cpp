#include "services/notifications/notification_service.hpp"
#include "util/outfit.hpp"
#include "views/view.hpp"

namespace big
{
	void view::outfit_editor()
	{
		static char outfit_name[MAX_PATH] = {};

		static std::string selected_folder, selected_file;
		static std::vector<std::string> outfits_folder, outfits;

		ImGui::BeginGroup();
		{
			ImGui::Text("Components");
			for (auto& item : outfit::components_t().items)
				components::button(std::format("Clear {}", item.label), [item] {
					PED::SET_PED_COMPONENT_VARIATION(self::ped, item.id, 0, 0, PED::GET_PED_PALETTE_VARIATION(self::ped, item.id));
				});
		}
		ImGui::EndGroup();
		ImGui::SameLine();
		ImGui::BeginGroup();
		{
			ImGui::Text("Props");
			for (auto& item : outfit::props_t().items)
				components::button(std::format("Clear {}", item.label), [item] {
					PED::CLEAR_PED_PROP(self::ped, item.id, 0);
				});
		}
		ImGui::EndGroup();

		ImGui::Spacing();

		ImGui::SetNextItemWidth(300);
		components::input_text("###outfit_name", outfit_name, sizeof(outfit_name));
		ImGui::SameLine();
		components::button("Save Current", [] {
			std::string str = outfit_name;
			auto folder     = outfit::get_folder(selected_folder);

			if (folder.get_file(str + ".json").exists())
			{
				g_notification_service->push_error("Save Outfit Failed", "File with same name already exists", true);
				return;
			}

			outfit::save_outfit(self::ped, str + ".json", selected_folder);
		});

		ImGui::Spacing();

		components::button("Refresh list", [] {
			outfits_folder.clear();
			outfits.clear();

			for (const auto& directory_entry : std::filesystem::directory_iterator(outfit::get_folder().get_path()))
				if (directory_entry.is_directory())
					outfits_folder.push_back(directory_entry.path().filename().generic_string());

			for (const auto& directory_entry : std::filesystem::directory_iterator(outfit::get_folder(selected_folder).get_path()))
				if (directory_entry.path().extension() == ".json")
					outfits.push_back(directory_entry.path().filename().generic_string());
		});
		ImGui::SameLine();
		components::button("Apply Selected", [] {
			if (selected_file.size())
			{
				auto filePath = outfit::get_folder(selected_folder).get_file(selected_file).get_path();

				if (std::filesystem::exists(filePath))
				{
					std::ifstream i(filePath);
					nlohmann::json j;
					i >> j;
					outfit::apply_outfit(j);
				}
				else
					g_notification_service->push_warning("Apply Outfit Failed", "File does not exist.");
			}
		});
		ImGui::SameLine();
		components::button("Delete Selected", [] {
			if (selected_file.size())
			{
				auto filePath = outfit::get_folder(selected_folder).get_file(selected_file).get_path();

				if (std::filesystem::exists(filePath))
					std::filesystem::remove(filePath);
				else
					g_notification_service->push_warning("Delete Outfit Failed", "File does not exist.");

				selected_file = "";
			}
		});

		ImGui::SetNextItemWidth(300.f);
		auto folder_display = selected_folder.empty() ? "Root" : selected_folder.c_str();
		if (ImGui::BeginCombo("Folder###folder_list", folder_display))
		{
			if (ImGui::Selectable("Root", selected_folder.empty()))
				selected_folder.clear();

			for (auto& folder_name : outfits_folder)
				if (ImGui::Selectable(folder_name.c_str(), selected_folder == folder_name))
					selected_folder = folder_name;

			ImGui::EndCombo();
		}

		if (ImGui::BeginListBox("###file_list", ImVec2(300, 300)))
		{
			for (auto& outfit : outfits)
				if (ImGui::Selectable(outfit.c_str(), outfit == selected_file))
					selected_file = outfit;
			ImGui::EndListBox();
		}
	}
}