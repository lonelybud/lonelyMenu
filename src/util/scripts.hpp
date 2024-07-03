#pragma once
#include "core/vars.hpp"
#include "gta/net_array.hpp"
#include "gta/script_handler.hpp"
#include "gta_util.hpp"
#include "packet.hpp"
#include "pointers.hpp"
#include "script.hpp"
#include "services/notifications/notification_service.hpp"
#include "services/players/player_service.hpp"

namespace big::scripts
{
	inline const char* get_host_name(rage::joaat_t hash)
	{
		if (auto script = gta_util::find_script_thread(hash); script && script->m_net_component)
			if (auto host = ((CGameScriptHandlerNetComponent*)script->m_net_component)->get_host())
				return host->get_name();

		return "?";
	}

	inline CGameScriptHandlerNetComponent* get_script_handler_net_comp(rage::joaat_t hash)
	{
		if (auto script = gta_util::find_script_thread(hash); script && script->m_net_component)
			return ((CGameScriptHandlerNetComponent*)script->m_net_component);

		return nullptr;
	}

	// inline bool send_migration_event(rage::joaat_t hash)
	// {
	// 	if (auto script = gta_util::find_script_thread(hash); script && script->m_net_component)
	// 		if (auto comp = (CGameScriptHandlerNetComponent*)script->m_net_component; comp->get_host())
	// 		{
	// 			std::string curr_host = comp->get_host()->get_name();

	// 			for (int i = 0; !comp->is_local_player_host(); i++)
	// 			{
	// 				if (i > 200)
	// 					return false;

	// 				comp->send_host_migration_event(g_player_service->get_self()->get_net_game_player());
	// 				script::get_current()->yield(10ms);

	// 				if (!(script && script->m_net_component))
	// 					return false;
	// 			}

	// 			LOGF(INFO, "send_host_migration_event: {} current host: {}", script->m_name, curr_host);
	// 			return true;
	// 		}

	// 	return false;
	// }

	inline const std::optional<uint32_t> get_code_location_by_pattern(rage::scrProgram* program, const memory::pattern& pattern)
	{
		uint32_t code_size = program->m_code_size;
		for (uint32_t i = 0; i < (code_size - pattern.m_bytes.size()); i++)
		{
			for (uint32_t j = 0; j < pattern.m_bytes.size(); j++)
				if (pattern.m_bytes[j].has_value())
					if (pattern.m_bytes[j].value() != *program->get_code_address(i + j))
						goto incorrect;

			return i;
		incorrect:
			continue;
		}

		return std::nullopt;
	}

	// inline void force_script_host(rage::joaat_t hash)
	// {
	// 	if (*g_pointers->m_gta.m_is_session_started && !is_maintransition_script_active)
	// 	{
	// 		if (auto thread = gta_util::find_script_thread(hash); thread && thread->m_net_component)
	// 		{
	// 			auto net_component = reinterpret_cast<CGameScriptHandlerNetComponent*>(thread->m_net_component);

	// 			if (net_component->is_local_player_host())
	// 				return g_notification_service.push_error("Force Script Host", "Already Host!");

	// 			int num_synced_arrays = 0;

	// 			for (int i = 0; i < net_component->m_host_array_count; i++)
	// 				if ((g_pointers->m_gta.m_get_host_array_handler_by_index(net_component, i)->m_flags & 1) != 0)
	// 					num_synced_arrays++;

	// 			if (num_synced_arrays == net_component->m_host_array_count)
	// 			{
	// 				net_component->do_host_migration(g_player_service->get_self()->get_net_game_player(), 0xFFFF, true);

	// 				packet pack;
	// 				pack.write_message(rage::eNetMessage::MsgScriptVerifyHostAck);
	// 				net_component->m_script_handler->get_id()->serialize(&pack.m_buffer);
	// 				pack.write<bool>(true, 1);
	// 				pack.write<bool>(true, 1);
	// 				pack.write<std::uint16_t>(0xFFFF, 16);

	// 				for (auto& player : g_player_service->players())
	// 					if (player.second->get_net_game_player())
	// 						pack.send(player.second->get_net_game_player()->m_msg_id);

	// 				return g_notification_service.push_success("Force Script Host", "Done!");
	// 			}
	// 		}

	// 		return g_notification_service.push_error("Force Script Host", "Arrays not synced");
	// 	}

	// 	return g_notification_service.push_error("Force Script Host", "Not yet online?");
	// }
}
