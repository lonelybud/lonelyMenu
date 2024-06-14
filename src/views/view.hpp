#pragma once
#include "gui/components/components.hpp"

namespace big
{
	class view
	{
		inline static ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav;

	public:
		static void active_view();
		static void debug();
		static void esp_settings();
		static void outfit_editor();
		static void gui_settings();
		static void reaction_settings();
		static void protection_settings();
		static void heading();
		static void navigation();
		static void notifications();
		static void root();
		static void self();
		static void animations();
		static void ptfx_effects();
		static void network();
		static void blocked_players();
		static void known_players();
		static void recent_spoofed_host_tokens();
		static void players_lang();
		static void chat();
		static void vehicle();
		static void lsc();
		static void spawn_vehicle();
		static void pv();
		static void persist_car();
		static void spawned_vehicles();
		static void teleport();
		static void custom_teleport();
		static void view_player();
		static void players();
		static void weapons();
		static void context_menu();
		static void gta_data();
		static void gta_cache();
		static void misc();
	};
}
