#pragma once
#include "core/enums.hpp"

#include <imgui.h>

namespace big
{
	inline struct g_context_menu_t
	{
		bool enabled = false;

		uint8_t allowed_entity_types = 0;

		bool dead_entities = false;

		ImU32 selected_option_color = 4278255360;

		bool bounding_box_enabled = true;
		ImU32 bounding_box_color  = 4278255360;
	} g_context_menu{};
}