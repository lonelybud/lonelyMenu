#include "custom_teleport_service.hpp"

#include "services/notifications/notification_service.hpp"

namespace big
{
	std::filesystem::path custom_teleport_service::get_telelocations_file_path()
	{
		return g_file_manager.get_project_file("telelocations.json").get_path();
	}

	void custom_teleport_service::fetch_saved_locations()
	{
		all_saved_locations.clear();

		try
		{
			auto filePath = get_telelocations_file_path();

			if (std::filesystem::exists(filePath))
			{
				std::ifstream f(filePath);

				if (f.is_open())
				{
					all_saved_locations = nlohmann::json::parse(f);
					f.close();
				}
			}
		}
		catch (std::exception e)
		{
			LOG(WARNING) << e.what();
		}
	}

	void custom_teleport_service::save_new_location(const std::string& category, telelocation t)
	{
		if (const auto& pair = all_saved_locations.insert({category, {t}}); !pair.second)
			pair.first->second.push_back(t);

		std::ofstream file_out(get_telelocations_file_path(), std::ofstream::trunc | std::ofstream::binary);
		if (!file_out.is_open())
			return;

		nlohmann::json j = all_saved_locations;
		file_out << j.dump(4);
		file_out.close();

		g_notification_service.push_success("Custom Teleport", std::format("Succesfully saved location {}", t.name));
	}

	void custom_teleport_service::delete_saved_location(const std::string& category, const std::string& location_name)
	{
		const auto& it = all_saved_locations.find(category);
		if (it == all_saved_locations.end())
			return;

		std::erase_if(it->second, [location_name](telelocation t) {
			return t.name == location_name;
		});

		if (!it->second.size())
			all_saved_locations.erase(category);

		std::ofstream file_out(get_telelocations_file_path(), std::ofstream::trunc | std::ofstream::binary);
		if (!file_out.is_open())
			return;

		nlohmann::json j = all_saved_locations;
		file_out << j.dump(4);
		file_out.close();
	}

	std::vector<telelocation> custom_teleport_service::saved_telelocations_filtered_list(std::string& filter)
	{
		std::vector<telelocation> filterlist{};
		std::string _filter = filter;
		std::transform(_filter.begin(), _filter.end(), _filter.begin(), ::tolower);

		for (auto& loc : all_saved_locations | std::views::values | std::views::join)
		{
			std::string loc_name = loc.name;
			std::transform(loc_name.begin(), loc_name.end(), loc_name.begin(), ::tolower);
			if (loc_name.find(_filter) != std::string::npos)
				filterlist.push_back(loc);
		}

		return filterlist;
	}
}
