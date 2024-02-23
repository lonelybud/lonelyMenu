#include "services/mobile/mobile_service.hpp"
#include "views/view.hpp"

namespace big
{
	void view::pv()
	{
		static bool delivering_veh;
		static std::string search_pv;

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
			std::transform(search_pv.begin(), search_pv.end(), search_pv.begin(), ::tolower);

		ImGui::SameLine();
		ImGui::Text(" Total: %d cars", g_mobile_service->personal_vehicles.size());

		ImGui::Spacing();

		if (!delivering_veh && ImGui::BeginListBox("###personal_veh_list", {300, 300}))
		{
			if (g_mobile_service->personal_vehicles.empty())
				ImGui::Text("No personal vehicles found, \nare you online?");
			else
				for (const auto& it : g_mobile_service->personal_vehicles)
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
