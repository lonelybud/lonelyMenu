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
			static bool idle = true, running = true, killed = true, paused = true, state_4 = true;

			if (g_pointers->m_gta.m_script_threads)
			{
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

				ImGui::Spacing();

				ImGui::BeginChild("ScrollingRegion", {600, 450});
				for (auto script : *g_pointers->m_gta.m_script_threads)
					if (script)
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
									components::button("Take Control", [script] {
										scripts::force_migration(script->m_script_hash);
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
				ImGui::EndChild();
			}
		}
		ImGui::EndTabItem();
	}
}
