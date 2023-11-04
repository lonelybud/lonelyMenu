#pragma once
#include "services/players/player_service.hpp"

namespace big
{
	inline struct g_player
	{
		bool spectating              = false;
		player_ptr host_to_auto_kick = nullptr;
	} g_player{};
}