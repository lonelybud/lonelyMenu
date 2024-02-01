#pragma once

#include "settings/esp.hpp"
#include "settings/menu.hpp"
#include "settings/window.hpp"

namespace big
{
	class menu_settings
	{
	private:
		file save_file;
		nlohmann::json last_settings;

		bool deep_compare(nlohmann::json&, const nlohmann::json&);
		void save();

	public:
		void init(const file& save_file);
		void attempt_save();

		// all the settings to collectively persist in one file
		g_menu_t& menu     = g_menu;
		g_window_t& window = g_window;
		g_esp_t& esp       = g_esp;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(menu_settings, menu, window, esp)
	};

	inline auto g_menu_settings = menu_settings();
}
