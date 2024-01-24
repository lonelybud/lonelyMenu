#include "services/mobile/mobile_service.hpp"
#include "util/mobile.hpp"
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

		ImGui::SetNextItemWidth(200);
		if (components::input_text_with_hint("###search_pv", "search", search_pv))
			search_pv = toLowercase(search_pv);
		ImGui::SameLine();
		ImGui::Text(" Total: %d cars", g_mobile_service->personal_vehicles().size());

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

		ImGui::Spacing();

		components::button("Request Avenger", [] {
			mobile::services::request_avenger();
		});
		components::button("Request Kosatka", [] {
			mobile::services::request_kosatka();
		});
		components::button("Request M.O.C", [] {
			mobile::services::request_mobile_operations_center();
		});
		components::button("Request Terrorbyte", [] {
			mobile::services::request_terrorbyte();
		});
		components::button("Request Acid Lab", [] {
			mobile::services::request_acidlab();
		});
		components::button("Request Acid Bike", [] {
			mobile::services::request_acidlab_bike();
		});
	}
}
