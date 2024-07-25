#include "notification_service.hpp"
#include "logger/logger.hpp"

namespace big
{
	void notification_service::push(const std::string& title, const std::string& message, bool log)
	{
		push({title, message, NotificationType::INFO});
		if (log)
			LOG(VERBOSE) << title << ": " << message;
	}

	void notification_service::push_warning(const std::string& title, const std::string& message, bool log)
	{
		push({title, message, NotificationType::WARNING, 7s});
		if (log)
			LOG(WARNING) << title << ": " << message;
	}

	void notification_service::push_error(const std::string& title, const std::string& message, bool log)
	{
		push({title, message, NotificationType::DANGER, 7s});
		if (log)
			LOG(FATAL) << title << ": " << message;
	}

	void notification_service::push_success(const std::string& title, const std::string& message, bool log)
	{
		push({title, message, NotificationType::SUCCESS, 7s});
		if (log)
			LOG(INFO) << title << ": " << message;
	}

	std::vector<notification> notification_service::get()
	{
		// remove old notifications
		for (auto it = m_notifications.begin(); it != m_notifications.end();)
		{
			if (it->second.should_be_destroyed())
			{
				it = m_notifications.erase(it++);

				continue;
			}
			++it;
		}

		// grab only the notifications, no need for the keys
		auto sorted = m_notifications | std::views::values | std::ranges::to<std::vector<notification>>();
		std::sort(sorted.begin(), sorted.end(), [](auto const& a, auto const& b) {
			// inverse sorting, highest remaining time goes to top
			return a.destroy_time() > b.destroy_time();
		});
		return sorted;
	}

	void notification_service::push(notification n)
	{
		const auto [pair, inserted] = m_notifications.insert({n.identifier(), n});
		if (!inserted)
		{
			pair->second.reset();
		}
	}

}
