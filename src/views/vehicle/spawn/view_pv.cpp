#include "services/mobile/mobile_service.hpp"
#include "util/mobile.hpp"
#include "views/view.hpp"

namespace big
{
	void view::pv()
	{
		static bool delivering_veh;

		if (delivering_veh && components::button("Reset delivering state"))
			delivering_veh = false;

		components::button("Refresh", [] {
			g_mobile_service->register_vehicles();
		});

		if (!delivering_veh && ImGui::BeginListBox("###personal_veh_list", {200, 300}))
		{
			if (g_mobile_service->personal_vehicles().empty())
				ImGui::Text("No personal vehicles found, \nare you online?");
			else
				for (const auto& it : g_mobile_service->personal_vehicles())
				{
					const auto& personal_veh = it.second;
					if (!personal_veh->is_blacklisted_vehicle())
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
