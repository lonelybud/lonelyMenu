#include "gui/components/components.hpp"
#include "pointers.hpp"
#include "util/scripts.hpp"

#include <imgui.h>

namespace big
{
	static const char* thread_states = "RUNNING\0WAITING\0KILLED\0PAUSED\0STATE_4\0";

	inline void view_debug_threads()
	{
		if (ImGui::BeginTabItem("Threads"))
		{
			if (g_pointers->m_gta.m_script_threads)
			{
				ImGui::Text(std::format("Number of threads: {}", g_pointers->m_gta.m_script_threads->size()).c_str());
				ImGui::Spacing();
				ImGui::BeginChild("ScrollingRegion", {600, 500});
				for (auto script : *g_pointers->m_gta.m_script_threads)
					if (script)
					{
						if (script->m_context.m_state != rage::eThreadState::killed && script->m_context.m_stack_size == 0)
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
								ImGui::Text(std::format("Script Host: {}", host->get_name()).c_str());
								if (!net_handler->is_local_player_host())
								{
									ImGui::SameLine();
									components::button("Take Control", [script] {
										scripts::force_migration(script->m_name, script->m_script_hash);
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
