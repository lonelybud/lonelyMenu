#pragma once
#include "views/view.hpp"

namespace big
{
	enum class tabs
	{
		NONE,

		SELF,
		WEAPONS,
		TELEPORT,
		OUTFIT_EDITOR,
		ANIMATIONS,

		VEHICLE,
		LSC,
		SPAWN_VEHICLE,
		PV,
		SPAWNED_VEHICLES,

		MISC,
		PTFX_EFFECTS,

		NETWORK,
		BLOCKED_PLAYERS,
		KNOWN_PLAYERS,
		RECENT_SPOOFED_HOST_TOKENS,
		PLAYER_LANGS,
		CHAT,
		SETTINGS,
		ESP_SETTINGS,
		GTA_CACHE_SETTINGS,
		GUI_SETTINGS,
		REACTION_SETTINGS,
		PROTECTION_SETTINGS,
		DEBUG,

		PLAYER
	};

	struct navigation_struct
	{
		char name[80]              = "";
		std::function<void()> func = nullptr;
		std::map<tabs, navigation_struct> sub_nav{};
		rage::joaat_t hash = rage::joaat(name);
	};


	class gui_service final
	{
		std::vector<tabs> current_tab{};

		std::map<tabs, navigation_struct> nav = {
		    {
		        tabs::SELF,
		        {
		            "Self",
		            view::self,
		            {
		                {tabs::OUTFIT_EDITOR, {"Outfit Editor", view::outfit_editor}},
		                {tabs::ANIMATIONS, {"Animations", view::animations}},
		            },
		        },
		    },
		    {
		        tabs::WEAPONS,
		        {"Weapons", view::weapons},
		    },
		    {
		        tabs::VEHICLE,
		        {
		            "Vehicle",
		            view::vehicle,
		            {
		                {tabs::SPAWN_VEHICLE, {"Spawn Vehicle", view::spawn_vehicle}},
		                {tabs::PV, {"Personal Vehicles", view::pv}},
		                {tabs::LSC, {"LSC", view::lsc}},
		                {tabs::SPAWNED_VEHICLES, {"Spawned Vehicles", view::spawned_vehicles}},
		            },
		        },
		    },
		    {
		        tabs::TELEPORT,
		        {"Teleport", view::teleport},
		    },
		    {
		        tabs::MISC,
		        {
		            "Misc",
		            view::misc,
		            {
		                {tabs::PTFX_EFFECTS, {"Ptfx Effects", view::ptfx_effects}},
		                {tabs::BLOCKED_PLAYERS, {"Blocked Players", view::blocked_players}},
		                {tabs::KNOWN_PLAYERS, {"Known Players", view::known_players}},
		                {tabs::RECENT_SPOOFED_HOST_TOKENS, {"Recent Spoofed Tokens", view::recent_spoofed_host_tokens}},
		                {tabs::PLAYER_LANGS, {"Players Language", view::players_lang}},
		                {tabs::CHAT, {"Chat", view::chat}},
		            },
		        },
		    },
		    {
		        tabs::NETWORK,
		        {"Network", view::network},
		    },
		    {
		        tabs::SETTINGS,
		        {
		            "Settings",
		            [this] {
			            this->set_selected(tabs::ESP_SETTINGS);
		            },
		            {
		                {tabs::ESP_SETTINGS, {"ESP", view::esp_settings}},
		                {tabs::GTA_CACHE_SETTINGS, {"GTA Cache", view::gta_cache}},
		                {tabs::GUI_SETTINGS, {"Menu", view::gui_settings}},
		                {tabs::REACTION_SETTINGS, {"Reactions", view::reaction_settings}},
		                {tabs::PROTECTION_SETTINGS, {"Protection", view::protection_settings}},
		                {tabs::DEBUG, {"Debug", view::debug}},
		            },
		        },
		    },
		    {
		        tabs::PLAYER,
		        {"", view::view_player},
		    },
		};

		void remove_from_nav_internal(std::map<big::tabs, big::navigation_struct>& nav, big::tabs existing_tab_id);

	public:
		gui_service();
		virtual ~gui_service();

		navigation_struct* get_selected();
		std::vector<tabs>& get_selected_tab();
		void set_selected(tabs);
		int get_rendered_navs_count();
		std::map<tabs, navigation_struct>& get_navigation();
		void remove_from_nav(tabs existing_tab_id);
	};

	inline gui_service* g_gui_service{};
}