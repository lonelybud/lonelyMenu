#include "settings.hpp"

namespace big
{
	void menu_settings::attempt_save()
	{
		const nlohmann::json j = *this;
		if (deep_compare(last_settings, j))
			save();
	}

	void menu_settings::init(const file& sv)
	{
		save_file = sv;

		std::ifstream file(save_file.get_path());

		if (save_file.exists())
			try
			{
				file >> last_settings;
				from_json(last_settings, *this); // load existing settings
				file.close();
			}
			catch (const std::exception& e)
			{
				LOG(FATAL) << e.what();
				file.close();
				return;
			}

		save(); // write back to disk
	}

	bool menu_settings::deep_compare(nlohmann::json& _last, const nlohmann::json& _current)
	{
		for (auto& e : _current.items())
			if (const std::string& key = e.key(); _last[key].is_object() ? deep_compare(_last[key], e.value()) : _last[key] != e.value())
				return true;
		return false;
	}

	void menu_settings::save()
	{
		std::ofstream file(save_file.get_path(), std::ios::out | std::ios::trunc);
		last_settings = *this;         // set last settings to existing settings
		file << last_settings.dump(4); // write existing settings to disk
		file.close();

		LOG(VERBOSE) << "Menu settings save success!";
	}
}