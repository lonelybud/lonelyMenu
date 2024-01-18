#pragma once
#include "core/enums.hpp"

namespace big
{
	class player;
	using player_ptr = std::shared_ptr<player>;

	class reaction
	{
	public:
		bool notify = true;
		bool log    = true;

		reaction_type type;
		reaction_sub_type sub_type;
		const char* m_event_name;
		const char* m_notify_message;
		bool notify_once;
		bool is_modder;
		bool other;
		int n_events_at_time;

		reaction(reaction_type type, reaction_sub_type sub_type, const char* event_name, const char* notify_message, bool notify_once, bool is_modder, bool other, int n_events_at_time = 3);
		virtual void process(player_ptr player);
	};
}