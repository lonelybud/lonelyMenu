#include "view_debug_info.cpp"
#include "view_debug_misc.cpp"
#include "view_debug_threads.cpp"
#include "views/view.hpp"

namespace big
{
	void view::debug()
	{
		ImGui::BeginTabBar("debug_tabbar");
		view_debug_misc();
		view_debug_info();
		view_debug_threads();
		ImGui::EndTabBar();
	}
}