#pragma once
#include "natives.hpp"

namespace big::outfit
{
	struct outfit_t
	{
		int id;
		std::string label;
		int drawable_id     = 0;
		int drawable_id_max = 0;
		int texture_id      = 0;
		int texture_id_max  = 0;
	};

	struct components_t
	{
		std::vector<outfit_t> items = {
		    // {0, "Face"},
		    {1, "Mask"},
		    // {2, "Hair"},
		    {3, "Torso"},
		    {4, "Leg"},
		    // {5, "Parachute / bag"},
		    {6, "Shoes"},
		    {7, "Accessory"},
		    {8, "Undershirt"},
		    // {9, "Kevlar"},
		    {10, "Badge"},
		    {11, "Torso 2"},
		};
	};

	struct props_t
	{
		std::vector<outfit_t> items = {
		    {0, "Hats"},
		    {1, "Glasses"},
		    {2, "Ears"},
		    {6, "Watches"},
		    {7, "Bracelets"},
		};
	};

	bool get_item(const std::vector<outfit_t>& items, int id)
	{
		return std::find_if(items.begin(), items.end(), [id](const outfit_t& item) {
			return item.id == id;
		});
	}

	inline void apply_outfit(nlohmann::json j)
	{
		outfit::components_t components;
		outfit::props_t props;

		for (auto& item : j["components"].items())
		{
			std::stringstream ss(item.key());
			int id = 0;
			ss >> id;

			if (auto it = outfit::get_item(components, id); it != components.end())
			{
				it->drawable_id = item.value()["drawable_id"];
				it->texture_id  = item.value()["texture_id"];
			}
		}
		for (auto& item : j["props"].items())
		{
			std::stringstream ss(item.key());
			int id = 0;
			ss >> id;

			if (auto it = outfit::get_item(props, id); it != props.end())
			{
				it->drawable_id = item.value()["drawable_id"];
				it->texture_id  = item.value()["texture_id"];
			}
		}

		set_self_comps_props(components, props);
	}

	inline void save_outfit(std::string filename)
	{
		outfit::components_t components;
		outfit::props_t props;
		auto model = ENTITY::GET_ENTITY_MODEL(self::ped);

		for (auto& item : components.items)
		{
			item.drawable_id = PED::GET_PED_DRAWABLE_VARIATION(self::ped, item.id);
			item.texture_id  = PED::GET_PED_TEXTURE_VARIATION(self::ped, item.id);
		}

		for (auto& item : props.items)
		{
			item.drawable_id = PED::GET_PED_PROP_INDEX(self::ped, item.id, 1);
			item.texture_id  = PED::GET_PED_PROP_TEXTURE_INDEX(self::ped, item.id);
		}

		nlohmann::json j;
		nlohmann::json j_components;
		nlohmann::json j_props;

		for (auto& item : components.items)
		{
			nlohmann::json tmp;
			tmp["drawable_id"]                    = item.drawable_id;
			tmp["texture_id"]                     = item.texture_id;
			j_components[std::to_string(item.id)] = tmp;
		}

		for (auto& item : props.items)
		{
			nlohmann::json tmp;
			tmp["drawable_id"]               = item.drawable_id;
			tmp["texture_id"]                = item.texture_id;
			j_props[std::to_string(item.id)] = tmp;
		}

		j["components"] = j_components;
		j["props"]      = j_props;
		j["model"]      = model;

		static folder saved_outfit_path = g_file_manager.get_project_folder("saved_outfits");
		std::ofstream o(saved_outfit_path.get_file(filename).get_path());
		o << std::setw(4) << j << std::endl;
	}
}