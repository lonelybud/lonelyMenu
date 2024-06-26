#include "ped_animations_service.hpp"

#include "gta/enums.hpp"
#include "services/notifications/notification_service.hpp"
#include "util/ped.hpp"

namespace big
{
	std::filesystem::path ped_animation_service::get_ped_animations_file_path()
	{
		return g_file_manager.get_project_file("ped_animations.json").get_path();
	}

	std::vector<ped_animation> ped_animation_service::saved_animations_filtered_list(std::string& filter)
	{
		std::vector<ped_animation> filterlist{};
		auto _filter = filter;
		std::transform(_filter.begin(), _filter.end(), _filter.begin(), ::tolower);

		for (auto& ped_animation : all_saved_animations | std::views::values | std::views::join)
		{
			auto pan = ped_animation.name;
			std::transform(pan.begin(), pan.end(), pan.begin(), ::tolower);

			if (pan.find(_filter) != std::string::npos)
				filterlist.push_back(ped_animation);
		}

		return filterlist;
	}

	bool ped_animation_service::fetch_saved_animations()
	{
		all_saved_animations.clear();

		auto path = get_ped_animations_file_path();
		std::ifstream file(path, std::ios::binary);

		try
		{
			if (!file.is_open())
				return false;

			nlohmann::json j;
			file >> j;
			all_saved_animations = j.get<std::map<std::string, std::vector<ped_animation>>>();

			return true;
		}
		catch (const std::exception& e)
		{
			LOG(WARNING) << "Failed fetching saved animations: " << e.what() << "\n";
		}

		return false;
	}

	bool ped_animation_service::save_new_animation(const std::string& category, ped_animation p)
	{
		const auto& pair = all_saved_animations.insert({category, {p}});
		if (!pair.second)
		{
			pair.first->second.push_back(p);
		}

		auto path = get_ped_animations_file_path();

		std::ofstream file_out(path, std::ofstream::trunc | std::ofstream::binary);
		if (!file_out.is_open())
			return false;

		nlohmann::json j = all_saved_animations;
		file_out << j.dump(4);
		file_out.close();

		g_notification_service.push_success("Animations", std::format("Succesfully saved location {}", p.name));

		return true;
	}

	bool ped_animation_service::delete_saved_animation(const std::string& category, ped_animation p)
	{
		auto path = get_ped_animations_file_path();

		const auto& it = all_saved_animations.find(category);
		if (it == all_saved_animations.end())
			return false;

		std::erase_if(it->second, [p](ped_animation p_) {
			return p_.name == p.name;
		});

		if (!it->second.size())
		{
			all_saved_animations.erase(category);
		}

		std::ofstream file_out(path, std::ofstream::trunc | std::ofstream::binary);
		if (!file_out.is_open())
			return false;

		nlohmann::json j = all_saved_animations;
		file_out << j.dump(4);
		file_out.close();

		return true;
	}

	ped_animation* ped_animation_service::get_animation_by_name(std::string name)
	{
		std::string str = "";
		for (auto& anim : saved_animations_filtered_list(str))
			if (anim.name == name)
				return &anim;

		return nullptr;
	}

	void ped_animation_service::play_saved_ped_animation(ped_animation p, Ped ped, float* rel_loc, float* rot)
	{
		float pos[3] = {0, 0, 0};

		if (rel_loc[0] || rel_loc[1] || rel_loc[2])
		{
			pos[0] = self::pos.x + rel_loc[0];
			pos[1] = self::pos.y + rel_loc[1];
			pos[2] = self::pos.z + rel_loc[2];
		}

		ped::ped_play_animation(ped, p.dict, p.anim, p.blendin, p.blendout, p.time_to_play, p.flags, p.start_phase, false, {pos[0], pos[1], pos[2]}, {rot[0], rot[1], rot[2]});
	}
}
