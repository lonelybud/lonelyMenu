#include "core/data/self.hpp"
#include "gta/joaat.hpp"
#include "hooking/hooking.hpp"
#include "core/vars.hpp"

namespace big
{
	const char* hooks::get_label_text(void* unk, const char* label)
	{
		const auto hash = rage::joaat(label);
		if (hash == "RESPAWN_W_MP"_J)
		{
			g_self.custom_coords[0] = self::pos.x;
			g_self.custom_coords[1] = self::pos.y;
			g_self.custom_coords[2] = self::pos.z;
		}

		return g_hooking->get_original<get_label_text>()(unk, label);
	}
}
