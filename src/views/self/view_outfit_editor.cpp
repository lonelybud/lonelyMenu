#include "backend/looped_command.hpp"
#include "core/data/misc.hpp"
#include "services/notifications/notification_service.hpp"
#include "util/outfit.hpp"
#include "views/view.hpp"

namespace big
{
	static outfit::components_t components;
	static outfit::props_t props;
	static std::vector<std::string> outfits_folder, outfits;
	static std::string selected_folder, selected_file;

	static void refresh_outfit_state()
	{
		for (auto& item : components.items)
		{
			item.drawable_id     = PED::GET_PED_DRAWABLE_VARIATION(self::ped, item.id);
			item.drawable_id_max = PED::GET_NUMBER_OF_PED_DRAWABLE_VARIATIONS(self::ped, item.id) - 1;

			item.texture_id     = PED::GET_PED_TEXTURE_VARIATION(self::ped, item.id);
			item.texture_id_max = PED::GET_NUMBER_OF_PED_TEXTURE_VARIATIONS(self::ped, item.id, item.drawable_id) - 1;
		}

		for (auto& item : props.items)
		{
			item.drawable_id     = PED::GET_PED_PROP_INDEX(self::ped, item.id, 1);
			item.drawable_id_max = PED::GET_NUMBER_OF_PED_PROP_DRAWABLE_VARIATIONS(self::ped, item.id) - 1;

			item.texture_id = PED::GET_PED_PROP_TEXTURE_INDEX(self::ped, item.id);
			item.texture_id_max = PED::GET_NUMBER_OF_PED_PROP_TEXTURE_VARIATIONS(self::ped, item.id, item.drawable_id) - 1;
		}
	}

	static void refresh_list()
	{
		outfits_folder.clear();
		outfits.clear();

		for (const auto& directory_entry : std::filesystem::directory_iterator(outfit::get_folder().get_path()))
			if (directory_entry.is_directory())
				outfits_folder.push_back(directory_entry.path().filename().generic_string());

		for (const auto& directory_entry : std::filesystem::directory_iterator(outfit::get_folder(selected_folder).get_path()))
			if (directory_entry.path().extension() == ".json")
				outfits.push_back(directory_entry.path().filename().generic_string());
	}

	void view::outfit_editor()
	{
		components::command_checkbox<"disable_clothing_validation">();
		ImGui::SameLine();
		ImGui::Checkbox("Save blend data", &g_misc.save_outfit_blend_data);
		ImGui::SameLine();
		ImGui::Checkbox("Apply hair", &g_misc.apply_outfit_hair);
		ImGui::SameLine();
		ImGui::Checkbox("Apply Blend only", &g_misc.apply_outfit_blend_data_only);
		ImGui::Spacing();

		components::button("Refresh State for current Outfit", [] {
			refresh_outfit_state();
		});

		ImGui::BeginGroup();
		for (auto& item : components.items)
		{
			ImGui::SetNextItemWidth(120);
			if (ImGui::InputInt(std::format("{} [0,{}]##1", item.label, item.drawable_id_max).c_str(), &item.drawable_id))
			{
				outfit::check_bounds_drawable(&item); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([item] {
					PED::SET_PED_COMPONENT_VARIATION(self::ped, item.id, item.drawable_id, 0, PED::GET_PED_PALETTE_VARIATION(self::ped, item.id));
					refresh_outfit_state();
				});
			}
		}
		ImGui::EndGroup();
		ImGui::SameLine();
		ImGui::BeginGroup();
		for (auto& item : components.items)
		{
			ImGui::SetNextItemWidth(120);
			if (ImGui::InputInt(std::format("{} {} [0,{}]##2", item.label, "TEX", item.texture_id_max).c_str(), &item.texture_id))
			{
				outfit::check_bounds_texture(&item); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([item] {
					PED::SET_PED_COMPONENT_VARIATION(self::ped, item.id, item.drawable_id, item.texture_id, PED::GET_PED_PALETTE_VARIATION(self::ped, item.id));
					refresh_outfit_state();
				});
			}
		}
		ImGui::EndGroup();
		ImGui::SameLine();
		ImGui::BeginGroup();
		{
			for (auto& item : props.items)
			{
				ImGui::SetNextItemWidth(120);
				if (ImGui::InputInt(std::format("{} [0,{}]##3", item.label, item.drawable_id_max).c_str(), &item.drawable_id))
				{
					outfit::check_bounds_drawable(&item); // The game does this on it's own but seems to crash if we call OOB values to fast.
					g_fiber_pool->queue_job([item] {
						if (item.drawable_id == -1)
							PED::CLEAR_PED_PROP(self::ped, item.id, 1);
						else
							PED::SET_PED_PROP_INDEX(self::ped, item.id, item.drawable_id, 0, TRUE, 1);

						refresh_outfit_state();
					});
				}
			}

			ImGui::Spacing();
			ImGui::Spacing();

			for (auto& item : props.items)
			{
				ImGui::SetNextItemWidth(120);
				if (ImGui::InputInt(std::format("{} {} [0,{}]##4", item.label, "TEX", item.texture_id_max).c_str(), &item.texture_id))
				{
					outfit::check_bounds_texture(&item); // The game does this on it's own but seems to crash if we call OOB values to fast.
					g_fiber_pool->queue_job([item] {
						PED::SET_PED_PROP_INDEX(self::ped, item.id, item.drawable_id, item.texture_id, TRUE, 1);
						refresh_outfit_state();
					});
				}
			}
		}
		ImGui::EndGroup();

		ImGui::Spacing();

		static char outfit_name[MAX_PATH] = {};
		auto folder_display               = selected_folder.empty() ? "Root" : selected_folder.c_str();

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
			refresh_list();
		});

		ImGui::Spacing();

		components::button("Refresh list", [] {
			refresh_list();
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

					if (!g_misc.disable_clothing_validation)
					{
						g_misc.disable_clothing_validation = true;
						dynamic_cast<looped_command*>(command::get(rage::joaat("disable_clothing_validation")))->refresh();
						g_notification_service->push_success("Clothing Validation", "Disabled automatically.");
					}

					outfit::apply_outfit(j);
					refresh_outfit_state();
					i.close();
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
				refresh_list();
			}
		});

		ImGui::SetNextItemWidth(300.f);
		if (ImGui::BeginCombo("Folder###folder_list", folder_display))
		{
			if (ImGui::Selectable("Root", selected_folder.empty()))
			{
				selected_folder.clear();
				refresh_list();
			}

			for (auto& folder_name : outfits_folder)
				if (ImGui::Selectable(folder_name.c_str(), selected_folder == folder_name))
				{
					selected_folder = folder_name;
					refresh_list();
				}

			ImGui::EndCombo();
		}

		static std::string search_outfit;
		ImGui::SetNextItemWidth(200);
		components::input_text_with_hint("###search_outfit", "search", search_outfit);

		if (ImGui::BeginListBox("###file_list", ImVec2(300, 300)))
		{
			for (auto& outfit : outfits)
				if ((search_outfit.length() ? (outfit.find(search_outfit) != std::string::npos) : true)
				    && ImGui::Selectable(outfit.c_str(), outfit == selected_file))
				{
					selected_file = outfit;
					strcpy(outfit_name, outfit.c_str());
				}
			ImGui::EndListBox();
		}
	}
}