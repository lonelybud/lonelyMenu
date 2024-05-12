#pragma once

namespace big
{
	struct weaponloadout_weapon_json
	{
		Hash weapon;
		Hash group;
		std::vector<Hash> component_array;
		int tint;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(weaponloadout_weapon_json, weapon, group, component_array, tint)
	};

	class persist_weapons
	{
		static std::vector<weaponloadout_weapon_json> get_loadout(std::string loadout_name);

	public:
		static void save_weapons(std::string loadout_name);
		static void give_player_loadout(std::string loadout_name);
		static std::vector<std::string> list_weapon_loadouts();
	};
}
