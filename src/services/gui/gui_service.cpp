#include "gui_service.hpp"

namespace big
{
	gui_service::gui_service()
	{
		g_gui_service = this;
	}

	gui_service::~gui_service()
	{
		g_gui_service = nullptr;
	}

	navigation_struct* gui_service::get_selected()
	{
		static navigation_struct tab_none = {"", nullptr};
		if (current_tab.empty() || current_tab.at(0) == tabs::NONE)
			return &tab_none;

		navigation_struct* current_nav = &nav.at(current_tab.at(0));
		if (current_tab.size() > 1)
		{
			for (const tabs& t : current_tab)
			{
				if (t == current_tab.at(0))
					continue;
				current_nav = &current_nav->sub_nav.at(t);
			}
		}

		return current_nav;
	}

	std::vector<tabs>& gui_service::get_selected_tab()
	{
		return current_tab;
	}

	void gui_service::set_selected(tabs tab)
	{
		if (current_tab.empty())
			return current_tab.push_back(tab);
		if (auto it = get_selected()->sub_nav.find(tab); it != get_selected()->sub_nav.end())
			current_tab.push_back(tab);
		else
		{
			current_tab.pop_back();
			set_selected(tab);
		}
	}

	// assuming only from level 1 to 2 navigations
	int gui_service::get_rendered_navs_count()
	{
		auto count = 0;
		for (std::pair<tabs, navigation_struct> nav_item : nav)
			if (nav_item.first == tabs::PLAYER)
				continue;
			else
			{
				++count;
				auto& tab = get_selected_tab();
				if (!tab.empty() && nav_item.first == tab.at(0))
					count += nav_item.second.sub_nav.size();
			}
		return count;
	}

	std::map<tabs, navigation_struct>& gui_service::get_navigation()
	{
		return nav;
	}

	void gui_service::remove_from_nav_internal(std::map<big::tabs, big::navigation_struct>& nav, big::tabs existing_tab_id)
	{
		std::erase_if(nav, [&](auto& nav_item) {
			if (nav_item.first == existing_tab_id)
			{
				set_selected(tabs::NONE);
				return true;
			}
			else
			{
				remove_from_nav_internal(nav_item.second.sub_nav, existing_tab_id);
			}

			return false;
		});
	}

	void gui_service::remove_from_nav(tabs existing_tab_id)
	{
		remove_from_nav_internal(get_navigation(), existing_tab_id);
	}
}
