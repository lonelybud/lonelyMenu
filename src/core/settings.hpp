#pragma once

#include "settings/esp.hpp"
#include "settings/notifications.hpp"
#include "settings/protections.hpp"
#include "settings/settings.hpp"
#include "settings/window.hpp"

namespace big
{
	class menu_settings
	{
	public:
		void init(const file& save_file);

		void attempt_save();
		bool load();
		bool write_default_config();

	private:
		bool deep_compare(nlohmann::json& current_settings, const nlohmann::json& default_settings, bool compare_value = false);
		bool save();

	private:
		file m_save_file;

		nlohmann::json m_default_options;
		nlohmann::json m_options;

	public:
		g_notifications_t& notifications = g_notifications;
		g_protections_t& protections     = g_protections;
		g_settings_t& settings           = g_settings;
		g_window_t& window               = g_window;
		g_esp_t& esp                     = g_esp;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(menu_settings, notifications, protections, settings, window, esp)
	};

	inline auto g_menu_settings = menu_settings();
}
