#include "services/mobile/mobile_service.hpp"
#include "thread_pool.hpp"
#include "util/strings.hpp"
#include "views/view.hpp"

namespace big
{
	void view::pv()
	{
		static bool delivering_veh;
		static std::string search_pv;
		static int tick_count = 61;

		if (tick_count > 60)
		{
			tick_count = 1;

			g_fiber_pool->queue_job([]() {
				g_mobile_service->register_vehicles();
			});
		}
		else
			++tick_count;

		if (delivering_veh)
		{
			ImGui::Text("Delivering....");
			ImGui::SameLine();
			if (components::button("Reset"))
				delivering_veh = false;
			ImGui::Spacing();
		}


		ImGui::Text(" Total: %d cars", g_mobile_service->personal_vehicles().size());
		ImGui::SameLine();
		if (components::button("Save PV List File"))
			g_thread_pool->push([] {
				if (auto& vehs = g_mobile_service->personal_vehicles(); vehs.size())
				{
					std::ofstream pv_list(
					    g_file_manager
					        .get_project_file(std::format("./pv_list_{}.txt", g_player_service->get_self()->get_name()))
					        .get_path(),
					    std::ios_base::out | std::ios_base::trunc);
					for (const auto& it : vehs)
						pv_list << it.first << std::endl;
					pv_list.close();
				}
			});
		ImGui::Spacing();

		ImGui::SetNextItemWidth(200);
		if (components::input_text_with_hint("###search_pv", "search", search_pv))
			search_pv = toLowercase(search_pv);
		ImGui::Spacing();

		if (!delivering_veh && ImGui::BeginListBox("###personal_veh_list", {300, 300}))
		{
			if (g_mobile_service->personal_vehicles().empty())
				ImGui::Text("No personal vehicles found, \nare you online?");
			else
				for (const auto& it : g_mobile_service->personal_vehicles())
				{
					const auto& personal_veh = it.second;

					if (search_pv.length() ? (it.first.find(search_pv) != std::string::npos) : true)
						components::selectable(it.first, false, [&personal_veh] {
							delivering_veh = true;
							personal_veh->summon();
							delivering_veh = false;
						});
				}

			ImGui::EndListBox();
		}
	}
}
