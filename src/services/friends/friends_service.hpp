#pragma once
#include "pointers.hpp"

#include <socialclub/FriendRegistry.hpp>

namespace big
{
	class friends_service final
	{
	public:
		static inline bool is_friend(rock_id rockstar_id)
		{
			for (uint32_t i = 0; i < g_pointers->m_gta.m_friend_registry->m_friend_count; i++)
				if (rockstar_id == g_pointers->m_gta.m_friend_registry->get(i)->m_rockstar_id)
					return true;
			return false;
		}
	};
}