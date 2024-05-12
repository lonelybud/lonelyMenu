#include "persist_weapons.hpp"

#include "gta/weapons.hpp"
#include "natives.hpp"
#include "services/gta_data/gta_data_service.hpp"
#include "services/notifications/notification_service.hpp"

namespace big
{
	static folder get_weapon_config_folder()
	{
		return g_file_manager.get_project_folder("./weapon_loadouts");
	}

	std::vector<weaponloadout_weapon_json> persist_weapons::get_loadout(std::string loadout_name)
	{
		auto file = get_weapon_config_folder().get_file(loadout_name);

		if (file.exists())
		{
			std::ifstream file_stream(file.get_path());
			try
			{
				nlohmann::json json_input_file;

				file_stream >> json_input_file;
				file_stream.close();
				return json_input_file;
			}
			catch (std::exception& e)
			{
				g_notification_service.push_warning("Persist Weapons", "Failed to load JSON file from disk.");
			}
		}
		else
			g_notification_service.push_warning("Persist Weapons", "File does not exist.");

		return {};
	}

	void persist_weapons::save_weapons(std::string loadout_name)
	{
		std::vector<weaponloadout_weapon_json> weapons;

		for (const auto& [name, weapon] : g_gta_data_service->weapons())
		{
			Hash weapon_hash = weapon.m_hash;
			if (weapon_hash != WEAPON_UNARMED && WEAPON::HAS_PED_GOT_WEAPON(self::ped, weapon_hash, FALSE))
			{
				weaponloadout_weapon_json weapon_loadout;
				weapon_loadout.weapon = weapon_hash;
				weapon_loadout.group  = rage::joaat("GROUP_" + weapon.m_weapon_type);
				weapon_loadout.tint   = WEAPON::GET_PED_WEAPON_TINT_INDEX(self::ped, weapon_hash);
				for (auto component : weapon.m_attachments)
				{
					Hash component_hash = rage::joaat(component);
					if (WEAPON::HAS_PED_GOT_WEAPON_COMPONENT(self::ped, weapon_hash, component_hash))
						weapon_loadout.component_array.push_back(component_hash);
				}
				weapons.push_back(weapon_loadout);
			}
		}

		nlohmann::json json_output_file = weapons;
		const auto file                 = get_weapon_config_folder().get_file(loadout_name + ".json");
		std::ofstream file_stream(file.get_path(), std::ios::out | std::ios::trunc);
		file_stream << json_output_file.dump(4);
		file_stream.close();
	}

	void persist_weapons::give_player_loadout(std::string loadout_name)
	{
		std::vector<weaponloadout_weapon_json> weapons = get_loadout(loadout_name);

		if (PED::IS_PED_DEAD_OR_DYING(self::ped, TRUE))
			return;

		WEAPON::REMOVE_ALL_PED_WEAPONS(self::ped, 0);

		for (auto weapon : weapons)
		{
			Hash weapon_hash = weapon.weapon;

			int maxAmmo;
			if (WEAPON::GET_MAX_AMMO(self::ped, weapon_hash, &maxAmmo) == FALSE)
				maxAmmo = 9999;
			WEAPON::GIVE_WEAPON_TO_PED(self::ped, weapon_hash, maxAmmo, FALSE, FALSE);

			if (weapon.group != GROUP_MELEE)
			{
				WEAPON::SET_PED_WEAPON_TINT_INDEX(self::ped, weapon_hash, weapon.tint);
				for (auto component : weapon.component_array)
					WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(self::ped, weapon_hash, component);
				//Mk II weapons require giving the weapon to the player twice for them to work, because reasons.
				WEAPON::GIVE_WEAPON_TO_PED(self::ped, weapon_hash, maxAmmo, FALSE, FALSE);
			}
		}
	}

	std::vector<std::string> persist_weapons::list_weapon_loadouts()
	{
		std::vector<std::string> file_paths;

		const auto file_path = get_weapon_config_folder();
		for (const auto& directory_entry : std::filesystem::directory_iterator(file_path.get_path()))
			if (directory_entry.path().extension() == ".json")
				file_paths.push_back(directory_entry.path().filename().generic_string());

		return file_paths;
	}
}