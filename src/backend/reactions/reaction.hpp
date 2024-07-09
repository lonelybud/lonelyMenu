#pragma once
#include "core/data/player.hpp"
#include "core/enums.hpp"

namespace big
{

	class reaction
	{
	public:
		bool notify = true;
		bool log    = true;

		reaction_type m_type;
		reaction_sub_type m_sub_type;
		const char* m_event_name;
		reaction_notif_type m_notif_type;
		player_type m_plyr_type;
		reaction_karma m_karma_type;

		reaction(reaction_type type, reaction_sub_type sub_type, const char* event_name, reaction_notif_type notif_type, player_type plyr_type, reaction_karma karma_type = reaction_karma::none);
		void process(player_ptr player, player_ptr target = nullptr);
	};
}