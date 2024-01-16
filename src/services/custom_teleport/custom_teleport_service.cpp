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

		auto path = get_telelocations_file_path();
		std::ifstream file(path, std::ios::binary);

		try
		{
			if (!file.is_open())
				return;

			nlohmann::json j;
			file >> j;
			all_saved_locations = j.get<std::map<std::string, std::vector<telelocation>>>();
		}
		catch (const std::exception& e)
		{
			LOG(WARNING) << "Failed fetching saved locations: " << e.what() << '\n';
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

		g_notification_service->push_success("Custom Teleport", std::format("Succesfully saved location {}", t.name));
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
}
