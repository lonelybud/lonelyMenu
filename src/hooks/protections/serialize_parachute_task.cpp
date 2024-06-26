#include "core/data/reactions.hpp"
#include "core/data/syncing_player.hpp"
#include "hooking/hooking.hpp"
#include "pointers.hpp"
#include "services/players/player_service.hpp"

#include <entities/CDynamicEntity.hpp>

namespace big
{
#pragma pack(push, 8)
	struct TaskObject
	{
		uint16_t m_net_id;
		rage::CDynamicEntity* m_entity;
	};
#pragma pack(pop)

	void hooks::serialize_parachute_task(__int64 info, rage::CSyncDataBase* serializer)
	{
		auto object = reinterpret_cast<TaskObject*>(info + 0x30);
		g_hooking->get_original<hooks::serialize_parachute_task>()(info, serializer);

		if (object->m_entity && object->m_entity->m_entity_type != 5)
		{
			g_pointers->m_gta.m_remove_reference(object->m_entity, &object->m_entity);

			if (m_syncing_player && m_syncing_player->is_valid())
				if (auto plyr = g_player_service->get_by_id(m_syncing_player->m_player_id))
					g_reactions.crash38.process(plyr);
			object->m_entity = nullptr;
			object->m_net_id = 0;
		}
	}
}
