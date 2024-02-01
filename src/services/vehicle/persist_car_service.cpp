#include "persist_car_service.hpp"

#include "base/CObject.hpp"
#include "core/data/vehicle.hpp"
#include "gta/vehicle_values.hpp"
#include "pointers.hpp"
#include "script.hpp"
#include "services/notifications/notification_service.hpp"
#include "util/misc.hpp"
#include "util/teleport.hpp"
#include "util/time.hpp"
#include "util/vehicle.hpp"
#include "vehicle/CVehicle.hpp"

namespace big
{
	void persist_car_service::save_vehicle(Vehicle vehicle, std::string file_name, std::string folder_name)
	{
		if (!ENTITY::DOES_ENTITY_EXIST(vehicle) || !ENTITY::IS_ENTITY_A_VEHICLE(vehicle))
		{
			g_notification_service->push_warning("Persist Car", "Tried to save a vehicle which does not exist");
			return;
		}

		if (!file_name.length())
			file_name = std::to_string(get_current_time_in_mill()).append(".json");

		const auto file = check_vehicle_folder(folder_name).get_file(file_name);
		std::ofstream file_stream(file.get_path(), std::ios::out | std::ios::trunc);
		file_stream << get_vehicle_json(vehicle).dump(4);
		file_stream.close();
	}

	void persist_car_service::load_vehicle(const std::optional<Vector3> spawn_coords, Ped ped)
	{
		if (!g_vehicle.persist_vehicle.file.empty())
		{
			const auto file = persist_car_service::check_vehicle_folder(g_vehicle.persist_vehicle.folder)
			                      .get_file(g_vehicle.persist_vehicle.file)
			                      .get_path();

			if (!std::filesystem::exists(file))
			{
				g_notification_service->push_warning("Persist Car", "File does not exist.");
				return;
			}

			std::ifstream file_stream(file);
			nlohmann::json vehicle_json;
			Vehicle vehicle;

			try
			{
				file_stream >> vehicle_json;
				vehicle = spawn_vehicle_full(vehicle_json, spawn_coords, ped);
			}
			catch (std::exception& e)
			{
				LOG(WARNING) << e.what();
			}

			file_stream.close();

			if (vehicle == -1)
				return;

			if (vehicle == 0)
				g_notification_service->push_error("Persist Car",
				    std::format("Unable to spawn {}", g_vehicle.persist_vehicle.file),
				    true);
			else
			{
				g_notification_service->push_success("Persist Car", std::format("Spawned {}", g_vehicle.persist_vehicle.file), true);
				g_vehicle.spawned_vehicles[vehicle] = {g_vehicle.persist_vehicle.file};

				if (g_vehicle.spawn_inside)
					teleport::into_vehicle(vehicle);

				// ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&vehicle);
			}
		}
		else
			g_notification_service->push_warning("Persist Car", "Select a file first");
	}

	void persist_car_service::delete_vehicle(std::string_view file_name)
	{
		if (const auto file = check_vehicle_folder(g_vehicle.persist_vehicle.folder).get_file(file_name); file.exists())
			std::filesystem::remove(file.get_path());
	}

	void persist_car_service::refresh_list()
	{
		g_vehicle.persist_vehicle.folders.clear();

		const auto file_path = check_vehicle_folder();
		for (const auto& directory_entry : std::filesystem::directory_iterator(file_path.get_path()))
			if (directory_entry.is_directory())
				g_vehicle.persist_vehicle.folders.push_back(directory_entry.path().filename().generic_string());

		g_vehicle.persist_vehicle.files.clear();

		const auto file_path2 = check_vehicle_folder(g_vehicle.persist_vehicle.folder);
		for (const auto& directory_entry : std::filesystem::directory_iterator(file_path2.get_path()))
			if (directory_entry.path().extension() == ".json")
				g_vehicle.persist_vehicle.files.push_back(directory_entry.path().filename().generic_string());
	}

	void persist_car_service::clone_ped_car(Vehicle vehicle, const char* ped_name)
	{
		if (!ENTITY::IS_ENTITY_A_VEHICLE(vehicle))
			return;

		std::string model_name = vehicle::get_vehicle_model_name(vehicle);
		auto veh               = spawn_vehicle_full(get_vehicle_json(vehicle), std::nullopt, self::ped);

		if (veh == -1)
			return;

		if (veh == 0)
			g_notification_service->push_error("Clone Car", std::format("Failed to clone '{}'({})", model_name, ped_name), true);
		else
		{
			g_notification_service->push_success("Clone Car", std::format("Cloned '{}'({})", model_name, ped_name), true);
			g_vehicle.spawned_vehicles[veh] = {model_name};
			// ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&veh);
		}
	}

	Vehicle persist_car_service::spawn_vehicle_full(nlohmann::json vehicle_json, const std::optional<Vector3>& spawn_coords, Ped ped)
	{
		const Hash vehicle_hash = vehicle_json[vehicle_model_hash_key];

		Vector3 spawn_location;

		if (spawn_coords.has_value())
			spawn_location = spawn_coords.value();
		else if (auto loc = vehicle::get_spawn_location(vehicle_hash, ped); loc.has_value())
			spawn_location = loc.value();
		else
			return -1;

		auto vehicle = vehicle::spawn(vehicle_hash, spawn_location, 0);

		if (vehicle && vehicle != -1)
		{
			script::get_current()->yield();

			VEHICLE::SET_VEHICLE_MOD_KIT(vehicle, 0);

			VEHICLE::SET_VEHICLE_COLOURS(vehicle, vehicle_json[primary_color_key], vehicle_json[secondary_color_key]);

			if (!vehicle_json[custom_primary_color_key].is_null())
			{
				std::vector<int> primary_custom_color = vehicle_json[custom_primary_color_key];
				VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(vehicle, primary_custom_color[0], primary_custom_color[1], primary_custom_color[2]);
			}
			if (!vehicle_json[custom_secondary_color_key].is_null())
			{
				std::vector<int> secondary_custom_color = vehicle_json[custom_secondary_color_key];
				VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(vehicle, secondary_custom_color[0], secondary_custom_color[1], secondary_custom_color[2]);
			}

			VEHICLE::SET_VEHICLE_WINDOW_TINT(vehicle, vehicle_json[vehicle_window_tint_key]);
			VEHICLE::SET_VEHICLE_EXTRA_COLOURS(vehicle, vehicle_json[pearlescent_color_key], vehicle_json[wheel_color_key]);
			VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(vehicle, vehicle_json[tire_can_burst]);

			std::map<int, bool> vehicle_extras = vehicle_json[vehicle_extras_key];
			for (const auto& [extra, extra_enabled] : vehicle_extras)
				VEHICLE::SET_VEHICLE_EXTRA(vehicle, extra, extra_enabled);

			VEHICLE::SET_VEHICLE_WHEEL_TYPE(vehicle, vehicle_json[wheel_type_key]);

			if (!vehicle_json[vehicle_livery_key].is_null())
				VEHICLE::SET_VEHICLE_LIVERY(vehicle, vehicle_json[vehicle_livery_key]);

			if (VEHICLE::IS_THIS_MODEL_A_CAR(vehicle_hash) || VEHICLE::IS_THIS_MODEL_A_BIKE(vehicle_hash))
			{
				std::vector<int> neon_color   = vehicle_json[neon_color_key];
				std::vector<bool> neon_lights = vehicle_json[neon_lights_key];

				VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT(vehicle, vehicle_json[plate_text_key].get<std::string>().c_str());
				VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT_INDEX(vehicle, vehicle_json[plate_text_index_key]);
				VEHICLE::SET_DRIFT_TYRES(vehicle, vehicle_json[drift_tires]);
				VEHICLE::SET_VEHICLE_NEON_COLOUR(vehicle, neon_color[0], neon_color[1], neon_color[2]);
				for (int i = NEON_LEFT; i <= NEON_BACK; i++)
					VEHICLE::SET_VEHICLE_NEON_ENABLED(vehicle, i, neon_lights[i]);
				VEHICLE::SET_VEHICLE_EXTRA_COLOUR_5(vehicle, vehicle_json[interior_color_key]);
				VEHICLE::SET_VEHICLE_EXTRA_COLOUR_6(vehicle, vehicle_json[dash_color_key]);
				VEHICLE::SET_VEHICLE_XENON_LIGHT_COLOR_INDEX(vehicle, vehicle_json[headlight_color_key]);
			}

			for (int i = MOD_SPOILERS; i < MOD_LIGHTBAR; i++)
				if (!vehicle_json[mod_names[i]].is_null())
				{
					if (vehicle_json[mod_names[i]].is_array())
					{
						std::vector<int> mod = vehicle_json[mod_names[i]];
						VEHICLE::SET_VEHICLE_MOD(vehicle, i, mod[0], mod[1]);
					}
					else
					{
						if (i == MOD_TYRE_SMOKE)
						{
							std::vector<int> tire_smoke_color = vehicle_json[tire_smoke_color_key];
							VEHICLE::SET_VEHICLE_TYRE_SMOKE_COLOR(vehicle, tire_smoke_color[0], tire_smoke_color[1], tire_smoke_color[2]);
						}
						VEHICLE::TOGGLE_VEHICLE_MOD(vehicle, i, TRUE);
					}
				}

			vehicle::repair(vehicle);
		}

		return vehicle;
	}

	nlohmann::json persist_car_service::get_vehicle_json(Vehicle vehicle)
	{
		nlohmann::json vehicle_json;
		int primary_color, secondary_color;
		int pearlescent_color, wheel_color;
		Hash vehicle_hash = ENTITY::GET_ENTITY_MODEL(vehicle);
		std::map<int, bool> vehicle_extras;

		VEHICLE::GET_VEHICLE_COLOURS(vehicle, &primary_color, &secondary_color);
		VEHICLE::GET_VEHICLE_EXTRA_COLOURS(vehicle, &pearlescent_color, &wheel_color);

		for (int extra = MOD_EXTRA_14; extra <= MOD_EXTRA_0; ++extra)
			if (auto id = (extra - MOD_EXTRA_0) * -1; VEHICLE::DOES_EXTRA_EXIST(vehicle, id))
				vehicle_extras[id] = !VEHICLE::IS_VEHICLE_EXTRA_TURNED_ON(vehicle, id);

		vehicle_json[vehicle_model_hash_key] = vehicle_hash;
		vehicle_json[primary_color_key]      = primary_color;
		vehicle_json[secondary_color_key]    = secondary_color;

		if (VEHICLE::GET_IS_VEHICLE_PRIMARY_COLOUR_CUSTOM(vehicle))
		{
			int custom_primary_color[3]{};
			VEHICLE::GET_VEHICLE_CUSTOM_PRIMARY_COLOUR(vehicle, &custom_primary_color[0], &custom_primary_color[1], &custom_primary_color[2]);
			vehicle_json[custom_primary_color_key] = custom_primary_color;
		}

		if (VEHICLE::GET_IS_VEHICLE_SECONDARY_COLOUR_CUSTOM(vehicle))
		{
			int custom_secondary_color[3]{};
			VEHICLE::GET_VEHICLE_CUSTOM_SECONDARY_COLOUR(vehicle, &custom_secondary_color[0], &custom_secondary_color[1], &custom_secondary_color[2]);
			vehicle_json[custom_secondary_color_key] = custom_secondary_color;
		}

		vehicle_json[vehicle_window_tint_key] = VEHICLE::GET_VEHICLE_WINDOW_TINT(vehicle);
		vehicle_json[pearlescent_color_key]   = pearlescent_color;
		vehicle_json[wheel_color_key]         = wheel_color;
		vehicle_json[tire_can_burst]          = VEHICLE::GET_VEHICLE_TYRES_CAN_BURST(vehicle);
		vehicle_json[vehicle_extras_key]      = vehicle_extras;
		vehicle_json[wheel_type_key]          = VEHICLE::GET_VEHICLE_WHEEL_TYPE(vehicle);

		if ((VEHICLE::GET_VEHICLE_LIVERY_COUNT(vehicle) > 1) && VEHICLE::GET_VEHICLE_LIVERY(vehicle) >= 0)
			vehicle_json[vehicle_livery_key] = VEHICLE::GET_VEHICLE_LIVERY(vehicle);

		if (VEHICLE::IS_THIS_MODEL_A_CAR(vehicle_hash) || VEHICLE::IS_THIS_MODEL_A_BIKE(vehicle_hash))
		{
			bool neon_lights[4]{};
			for (int i = NEON_LEFT; i <= NEON_BACK; i++)
				neon_lights[i] = VEHICLE::GET_VEHICLE_NEON_ENABLED(vehicle, i);

			int neon_color[3]{};
			VEHICLE::GET_VEHICLE_NEON_COLOUR(vehicle, &neon_color[0], &neon_color[1], &neon_color[2]);

			int interior_color, dashboard_color;
			VEHICLE::GET_VEHICLE_EXTRA_COLOUR_5(vehicle, &interior_color);
			VEHICLE::GET_VEHICLE_EXTRA_COLOUR_6(vehicle, &dashboard_color);

			vehicle_json[plate_text_key]       = VEHICLE::GET_VEHICLE_NUMBER_PLATE_TEXT(vehicle);
			vehicle_json[plate_text_index_key] = VEHICLE::GET_VEHICLE_NUMBER_PLATE_TEXT_INDEX(vehicle);
			vehicle_json[drift_tires]          = VEHICLE::GET_DRIFT_TYRES_SET(vehicle);
			vehicle_json[neon_color_key]       = neon_color;
			vehicle_json[neon_lights_key]      = neon_lights;
			vehicle_json[interior_color_key]   = interior_color;
			vehicle_json[dash_color_key]       = dashboard_color;
			vehicle_json[headlight_color_key]  = VEHICLE::GET_VEHICLE_XENON_LIGHT_COLOR_INDEX(vehicle);
		}

		for (int i = MOD_SPOILERS; i <= MOD_LIGHTBAR; i++)
			if (VEHICLE::GET_VEHICLE_MOD(vehicle, i) != -1)
			{
				int vehicle_mod[2] = {VEHICLE::GET_VEHICLE_MOD(vehicle, i), VEHICLE::GET_VEHICLE_MOD_VARIATION(vehicle, i)};
				vehicle_json[mod_names[i]] = vehicle_mod;
			}
			else if (VEHICLE::IS_TOGGLE_MOD_ON(vehicle, i))
			{
				vehicle_json[mod_names[i]] = "TOGGLE";

				if (i == MOD_TYRE_SMOKE)
				{
					int tire_smoke_color[3]{};
					VEHICLE::GET_VEHICLE_TYRE_SMOKE_COLOR(vehicle, &tire_smoke_color[0], &tire_smoke_color[1], &tire_smoke_color[2]);
					vehicle_json[tire_smoke_color_key] = tire_smoke_color;
				}
			}

		return vehicle_json;
	}

	big::folder persist_car_service::check_vehicle_folder(std::string folder_name)
	{
		return g_file_manager.get_project_folder("./saved_json_vehicles/" + folder_name);
	}
}
