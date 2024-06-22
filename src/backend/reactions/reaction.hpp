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

		reaction_type type;
		reaction_sub_type sub_type;
		const char* m_event_name;
		bool notify_once;
		bool is_modder;
		bool other;
		reaction_karma m_karma_type;
		int m_attempts_before_log;

		reaction(reaction_type type, reaction_sub_type sub_type, const char* event_name, bool notify_once, bool is_modder, bool other, reaction_karma karma_type = reaction_karma::none, int attempts_before_log = 0);
		virtual void process(player_ptr player, player_ptr target = nullptr, bool silent = false);
	};
}