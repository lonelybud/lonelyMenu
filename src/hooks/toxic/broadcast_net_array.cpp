#include "core/scr_globals.hpp"
#include "gta/net_array.hpp"
#include "hooking/hooking.hpp"
#include "services/players/player_service.hpp"

#include <script/globals/GlobalPlayerBD.hpp>

namespace big
{
	unsigned int hooks::broadcast_net_array(rage::netArrayHandlerBase* _this, CNetGamePlayer* target, rage::datBitBuffer* bit_buffer, uint16_t counter, uint32_t* elem_start, bool silent)
	{
		int orig_gsbd;
		auto plyr = g_player_service->get_by_id(target->m_player_id);

		bool need_to_use_end_session_kick =
		    plyr && plyr->is_valid() && plyr->trigger_end_session_kick && _this->m_array == scr_globals::gsbd.as<void*>();

		if (need_to_use_end_session_kick)
		{
			orig_gsbd                          = *scr_globals::gsbd.as<int*>();
			*scr_globals::gsbd.as<uint32_t*>() = 5;
			broadcast_net_array::m_patch->apply();
		}

		int ret = g_hooking->get_original<hooks::broadcast_net_array>()(_this, target, bit_buffer, counter, elem_start, silent);

		if (need_to_use_end_session_kick)
		{
			broadcast_net_array::m_patch->restore();
			*scr_globals::gsbd.as<int*>() = orig_gsbd;
		}

		return ret;
	}
}