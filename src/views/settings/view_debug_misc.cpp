#include "gui/components/components.hpp"
#include "natives.hpp"
#include "util/local_player.hpp"
#include "util/lua_script.cpp"
#include "util/pools.hpp"

namespace big
{
	inline void view_debug_misc()
	{
		if (ImGui::BeginTabItem("Misc"))
		{
			static bool imGui_demo = false;

			ImGui::Spacing();

			components::button("Die", [] {
				ENTITY::SET_ENTITY_HEALTH(self::ped, 0, 0);
			});
			ImGui::SameLine();
			components::button("Toggle Radar", [] {
				HUD::DISPLAY_RADAR(HUD::IS_RADAR_HIDDEN());
			});

			components::button("Skip Cutscene", [] {
				CUTSCENE::STOP_CUTSCENE_IMMEDIATELY();
			});
			ImGui::SameLine();
			components::button("Clear Attachments", [] {
				for (auto obj : pools::get_all_props())
				{
					auto object = g_pointers->m_gta.m_ptr_to_handle(obj);
					if (!object)
						break;

					if (!ENTITY::IS_ENTITY_ATTACHED_TO_ENTITY(self::ped, object))
						continue;

					ENTITY::DELETE_ENTITY(&object);
				}
			});

			components::button("Network Bail", [] {
				NETWORK::NETWORK_BAIL(16, 0, 0);
			});
			ImGui::SameLine();
			components::button("Rage Quit", [] {
				exit(0);
			});

			components::button("Unlock Bunker Research (Temp.)", [] {
				auto MPX = local_player::get_mp_prefix();

				for (int i = 0; i < 64; ++i)
				{
					for (int j = 0; j < 3; ++j)
						lua_script::stats::set_bool_masked(MPX + "DLCGUNPSTAT_BOOL" + std::to_string(j), true, i);

					for (int j = 0; j < 6; ++j)
						lua_script::stats::set_bool_masked(MPX + "GUNTATPSTAT_BOOL" + std::to_string(j), true, i);
				}
			});
			ImGui::SameLine();
			components::button("Unlock Bunker Research (Temp.) 2", [] {
				auto MPX          = local_player::get_mp_prefix();
				const int bitSize = 8;

				for (int j = 0; j < 64 / bitSize; ++j)
					for (int i = 0; i < 13; ++i)
						lua_script::stats::set_masked_int(MPX + "GUNRPSTAT_INT" + std::to_string(i), -1, j * bitSize, bitSize);
			});

			ImGui::Spacing();

			ImGui::Checkbox("ImGui Demo", &imGui_demo);

			if (imGui_demo)
				ImGui::ShowDemoWindow(&imGui_demo);

			ImGui::EndTabItem();
		}
	}
}
