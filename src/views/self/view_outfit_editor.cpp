#include "services/notifications/notification_service.hpp"
#include "util/outfit.hpp"
#include "views/view.hpp"

namespace big
{
	static folder get_folder()
	{
		return g_file_manager.get_project_folder("saved_outfits");
	}

	void view::outfit_editor()
	{
		static char outfit_name[MAX_PATH] = {};
		static std::vector<std::string> saved_outfits;
		static std::string selected_file;

		components::button("Refresh list", [] {
			saved_outfits.clear();
			for (const auto& directory_entry : std::filesystem::directory_iterator(get_folder().get_path()))
				saved_outfits.push_back(directory_entry.path().filename().generic_string());
		});

		ImGui::Spacing();

		ImGui::SetNextItemWidth(300);
		components::input_text("###outfit_name", outfit_name, sizeof(outfit_name));
		ImGui::SameLine();
		components::button("Save Current", [] {
			std::string str = outfit_name;
			auto folder     = get_folder();

			if (folder.get_file(str + ".json").exists())
			{
				g_notification_service->push_error("Save Outfit Failed", "File with same name already exists", true);
				return;
			}

			outfit::save_outfit(str + ".json");
		});

		ImGui::Spacing();

		components::button("Apply Selected", [] {
			if (selected_file.size())
			{
				auto filePath = get_folder().get_file(selected_file).get_path();

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
				auto filePath = get_folder().get_file(selected_file).get_path();

				if (std::filesystem::exists(filePath))
					std::filesystem::remove(filePath);
				else
					g_notification_service->push_warning("Delete Outfit Failed", "File does not exist.");

				selected_file = "";
			}
		});

		if (ImGui::BeginListBox("##outfit_editor_list_box", ImVec2(300, 300)))
		{
			for (auto& outfit : saved_outfits)
				if (ImGui::Selectable(outfit.c_str(), outfit == selected_file))
					selected_file = outfit;
			ImGui::EndListBox();
		}
	}
}