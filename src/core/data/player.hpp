#pragma once

namespace big
{
	class player;
	using player_ptr  = std::shared_ptr<player>;
	using rock_id     = int64_t;
	using rock_id_ptr = int64_t*;
}