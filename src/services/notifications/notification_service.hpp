#pragma once
#include "notification.hpp"

namespace big
{

	class notification_service final
	{
		std::unordered_map<std::size_t, notification> m_notifications;

	public:
		notification_service() = default;
		virtual ~notification_service() = default;

		void push(const std::string& title, const std::string& message, bool log = false);
		void push_warning(const std::string& title, const std::string& message, bool log = false);
		void push_error(const std::string& title, const std::string& message, bool log = false);
		void push_success(const std::string& title, const std::string& message, bool log = false);

		// cleans up old notifications from the map and returns a sorted list based on the destroy time
		std::vector<notification> get();

	private:
		void push(notification notification);
	};

	inline notification_service g_notification_service{};
}
