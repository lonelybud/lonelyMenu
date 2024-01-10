#pragma once
#include "natives.hpp"
#include "services/notifications/notification_service.hpp"

namespace big::outfit
{
	inline folder get_folder(std::string folder_name = "")
	{
		return g_file_manager.get_project_folder("./saved_outfits/" + folder_name);
	}

	struct outfit_t
	{
		int id;
		std::string label;
		int drawable_id     = 0;
		int texture_id      = 0;
		int drawable_id_max = 0;
		int texture_id_max  = 0;
	};

	struct components_t
	{
		std::vector<outfit_t> items = {
		    {0, "Face"},
		    {1, "Mask"},
		    // {2, "Hair"},
		    {3, "Torso", 15, 0},
		    {4, "Leg", 15, 0},
		    {5, "Parachute / bag"},
		    {6, "Shoes", 35, 0},
		    {7, "Accessory"},
		    {8, "Undershirt", 14, 0},
		    {9, "Kevlar"},
		    {10, "Badge"},
		    {11, "Torso 2", -1, 0},
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

	inline int get_item_index(const std::vector<outfit_t>& items, int id)
	{
		auto it = std::find_if(items.begin(), items.end(), [id](const outfit_t& item) {
			return item.id == id;
		});

		if (it != items.end())
			return std::distance(items.begin(), it);

		return -1;
	}

	inline void set_self_comps_props(outfit::components_t components, outfit::props_t props, Ped target = 0)
	{
		for (auto item : components.items)
		{
			auto draw    = target ? PED::GET_PED_DRAWABLE_VARIATION(target, item.id) : item.drawable_id;
			auto texture = target ? PED::GET_PED_TEXTURE_VARIATION(target, item.id) : item.texture_id;

			PED::SET_PED_COMPONENT_VARIATION(self::ped, item.id, draw, texture, PED::GET_PED_PALETTE_VARIATION(self::ped, item.id));
		}

		PED::CLEAR_ALL_PED_PROPS(self::ped, 0);

		for (auto& item : props.items)
		{
			auto draw    = target ? PED::GET_PED_PROP_INDEX(target, item.id, 1) : item.drawable_id;
			auto texture = target ? PED::GET_PED_PROP_TEXTURE_INDEX(target, item.id) : item.texture_id;

			PED::SET_PED_PROP_INDEX(self::ped, item.id, draw, texture, TRUE, 1);
		}
	}

	inline void apply_outfit(nlohmann::json j)
	{
		components_t components;
		props_t props;

		if (j["model"] != ENTITY::GET_ENTITY_MODEL(self::ped))
		{
			g_notification_service->push_error("Apply Oufit", "Model not same.", true);
			return;
		}

		for (auto& item : j["components"].items())
		{
			std::stringstream ss(item.key());
			int id = 0;
			ss >> id;

			if (auto index = get_item_index(components.items, id); index != -1)
			{
				components.items[index].drawable_id = item.value()["drawable_id"];
				components.items[index].texture_id  = item.value()["texture_id"];
			}
		}
		for (auto& item : j["props"].items())
		{
			std::stringstream ss(item.key());
			int id = 0;
			ss >> id;

			if (auto index = get_item_index(props.items, id); index != -1)
			{
				props.items[index].drawable_id = item.value()["drawable_id"];
				props.items[index].texture_id  = item.value()["texture_id"];
			}
		}

		set_self_comps_props(components, props);
	}

	inline void save_outfit(Ped ped, std::string filename, std::string folder)
	{
		outfit::components_t components;
		outfit::props_t props;
		auto model = ENTITY::GET_ENTITY_MODEL(ped);

		for (auto& item : components.items)
		{
			item.drawable_id = PED::GET_PED_DRAWABLE_VARIATION(ped, item.id);
			item.texture_id  = PED::GET_PED_TEXTURE_VARIATION(ped, item.id);
		}

		for (auto& item : props.items)
		{
			item.drawable_id = PED::GET_PED_PROP_INDEX(ped, item.id, 1);
			item.texture_id  = PED::GET_PED_PROP_TEXTURE_INDEX(ped, item.id);
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

		auto path = outfit::get_folder(folder).get_file(filename).get_path();
		std::ofstream o(path);
		o << std::setw(4) << j << std::endl;
		o.close();
	}

	inline void check_bounds_drawable(outfit_t* item)
	{
		if (item->drawable_id > item->drawable_id_max)
			item->drawable_id = item->drawable_id_max;
		if (item->drawable_id < -1)
			item->drawable_id = -1;
	}

	inline void check_bounds_texture(outfit_t* item)
	{
		if (item->texture_id > item->texture_id_max)
			item->texture_id = item->texture_id_max;
		if (item->texture_id < -1)
			item->texture_id = -1;
	}
}