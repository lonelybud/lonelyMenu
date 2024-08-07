#include "core/data/script_block_options.hpp"
#include "core/vars.hpp"
#include "gui/components/components.hpp"
#include "pointers.hpp"
#include "util/scripts.hpp"

#include <imgui.h>

namespace big
{
	static const char* thread_states = "IDLE\0RUNNING\0KILLED\0PAUSED\0STATE_4\0";

	inline void view_debug_threads()
	{
		if (ImGui::BeginTabItem("Threads"))
		{
			static std::string search;
			static bool idle = true, running = true, killed = true, paused = true, state_4 = true;
			static script_types_options script_types;

			if (g_pointers->m_gta.m_script_threads)
			{
				ImGui::SetNextItemWidth(300);
				if (components::input_text_with_hint("###search_thread", "search", search))
					std::transform(search.begin(), search.end(), search.begin(), ::tolower);
				ImGui::SameLine();
				ImGui::Text("Number of threads: %d", g_pointers->m_gta.m_script_threads->size());

				ImGui::Checkbox("idle", &idle);
				ImGui::SameLine();
				ImGui::Checkbox("running", &running);
				ImGui::SameLine();
				ImGui::Checkbox("killed", &killed);
				ImGui::SameLine();
				ImGui::Checkbox("paused", &paused);
				ImGui::SameLine();
				ImGui::Checkbox("state_4", &state_4);

				ImGui::Checkbox("interiors", &script_types.interiors);
				ImGui::SameLine();
				ImGui::Checkbox("stores", &script_types.stores);
				ImGui::SameLine();
				ImGui::Checkbox("lsc", &script_types.lsc);
				ImGui::SameLine();
				ImGui::Checkbox("ammunation", &script_types.ammunation);
				ImGui::SameLine();
				ImGui::Checkbox("gang_attacks", &script_types.gang_attacks);

				ImGui::Spacing();

				ImGui::BeginChild("ScrollingRegion", {600, 450});
				for (auto script : *g_pointers->m_gta.m_script_threads)
				{
					if (!script)
						continue;

					std::string script_name = script->m_name;
					std::transform(script_name.begin(), script_name.end(), script_name.begin(), ::tolower);

					if (script_name.find(search) != std::string::npos)
					{
						if (script->m_context.m_state != rage::eThreadState::killed && script->m_context.m_stack_size == 0)
							continue;

						if (script->m_context.m_state == rage::eThreadState::idle && !idle)
							continue;
						if (script->m_context.m_state == rage::eThreadState::running && !running)
							continue;
						if (script->m_context.m_state == rage::eThreadState::killed && !killed)
							continue;
						if (script->m_context.m_state == rage::eThreadState::unk_3 && !paused)
							continue;
						if (script->m_context.m_state == rage::eThreadState::unk_4 && !state_4)
							continue;

						if (script_types.interiors && !script_block_options::interiors.contains(script->m_script_hash))
							continue;
						if (script_types.stores && !script_block_options::stores.contains(script->m_script_hash))
							continue;
						if (script_types.lsc && script_block_options::hash_lsc != script->m_script_hash)
							continue;
						if (script_types.ammunation && script_block_options::hash_ammunation != script->m_script_hash)
							continue;
						if (script_types.gang_attacks && script_block_options::hash_gang_attacks != script->m_script_hash)
							continue;

						ImGui::PushID(script->m_context.m_thread_id);

						if (script->m_context.m_state == rage::eThreadState::killed)
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.1f, 0.1f, 1.f));
						ImGui::Text(script->m_name);
						if (script->m_context.m_state == rage::eThreadState::killed)
							ImGui::PopStyleColor();

						if (auto net_handler = reinterpret_cast<CGameScriptHandlerNetComponent*>(script->m_net_component))
							if (auto host = net_handler->get_host())
							{
								ImGui::SameLine();
								ImGui::Text("Script Host: %s", host->get_name());
								if (!net_handler->is_local_player_host())
								{
									ImGui::SameLine();
									components::button("Take Control", [net_handler] {
										net_handler->send_host_migration_event(g_player_service->get_self()->get_net_game_player());
									});
								}
							}

						ImGui::SameLine();
						ImGui::SetNextItemWidth(100);
						ImGui::Combo("##state", (int*)&script->m_context.m_state, thread_states);

						if (script->m_context.m_state != rage::eThreadState::killed)
						{
							ImGui::SameLine();
							if (ImGui::Button("Kill"))
							{
								if (script->m_context.m_stack_size != 0)
									script->kill();

								script->m_context.m_state = rage::eThreadState::killed;
							}
						}

						ImGui::PopID();
						ImGui::Spacing();
					}
				}
				ImGui::EndChild();
			}

			ImGui::EndTabItem();
		}
	}
}
