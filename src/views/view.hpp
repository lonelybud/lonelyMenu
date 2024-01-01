#pragma once
#include "esp/view_esp.hpp"
#include "gui/components/components.hpp"

//Percentage of window space
constexpr auto listbox_width  = 0.5f;
constexpr auto listbox_height = 0.2f;

namespace big
{

	inline ImVec2 get_listbox_dimensions()
	{
		return {750 * listbox_width, 1000 * listbox_height};
	}

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
		static void bad_players();
		static void chat();
		static void vehicle();
		static void lsc();
		static void spawn_vehicle();
		static void pv();
		static void persist_car();
		static void teleport();
		static void view_player();
		static void players();
		static void weapons();
		static void context_menu();
		static void gta_data();
		static void gta_cache();
	};
}
