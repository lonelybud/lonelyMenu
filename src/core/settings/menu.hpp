#pragma once

namespace big
{
	inline struct g_menu_t
	{
		int menu_toggle = VK_INSERT;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(g_menu_t, menu_toggle)
	} g_menu{};
}
