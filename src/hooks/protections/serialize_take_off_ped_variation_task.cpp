#include "core/data/reactions.hpp"
#include "core/data/syncing_player.hpp"
#include "gta/joaat.hpp"
#include "hooking/hooking.hpp"
#include "services/players/player_service.hpp"

#include <netsync/nodes/task/ClonedTakeOffPedVariationInfo.hpp>

namespace big
{
	constexpr uint32_t valid_parachute_models[] = {
	    "p_parachute_s"_J,
	    "vw_p_para_bag_vine_s"_J,
	    "reh_p_para_bag_reh_s_01a"_J,
	    "xm_prop_x17_scuba_tank"_J,
	    "lts_p_para_bag_pilot2_s"_J,
	    "lts_p_para_bag_lts_s"_J,
	    "p_para_bag_tr_s_01a"_J,
	    "p_para_bag_xmas_s"_J,
	};

	bool is_valid_parachute_model(rage::joaat_t hash)
	{
		for (auto& model : valid_parachute_models)
			if (model == hash)
				return true;

		return false;
	}

	void hooks::serialize_take_off_ped_variation_task(ClonedTakeOffPedVariationInfo* info, rage::CSyncDataBase* serializer)
	{
		g_hooking->get_original<hooks::serialize_take_off_ped_variation_task>()(info, serializer);
		if (!is_valid_parachute_model(info->m_prop_hash))
		{
			if (m_syncing_player && m_syncing_player->is_valid())
				if (auto plyr = g_player_service->get_by_id(m_syncing_player->m_player_id))
					g_reactions.crash39.process(plyr);
			info->m_prop_hash = 0;
		}
	}
}
