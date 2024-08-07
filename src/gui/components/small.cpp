#include "core/settings/window.hpp"
#include "gui/components/components.hpp"

namespace big
{
	void components::small_text(const std::string_view text)
	{
		components::custom_text(text.data(), g_window.font_small);
	}
}