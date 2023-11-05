#include "natives.hpp"
#include "util/delete_entity.hpp"
#include "util/ped.hpp"
#include "util/teleport.hpp"
#include "views/view.hpp"

namespace big
{
	struct veh_details
	{
		Vehicle veh;
		const char* player_name;
		const char* veh_name;
	};

	void view::spawned_vehicles()
	{
		static std::vector<veh_details> spawned_vehs;
		static int tick_count = 61;

		++tick_count;

		if (tick_count > 60)
		{
			tick_count = 0;
			g_fiber_pool->queue_job([] {
				std::vector<veh_details> temp_vehs;

				for (auto& [_, veh] : self::spawned_vehicles)
				{
					veh_details details = {veh, "[]", "[DEAD]"};
					if (ENTITY::DOES_ENTITY_EXIST(veh) && !ENTITY::IS_ENTITY_DEAD(veh, 0))
					{
						details.veh_name = VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(ENTITY::GET_ENTITY_MODEL(veh));

						auto ped = VEHICLE::GET_PED_IN_VEHICLE_SEAT(veh, -1, 0);
						if (ped == self::ped)
							details.player_name = "You";
						else if (auto player = ped::get_player_from_ped(ped))
							details.player_name = player->get_name();
					}
					temp_vehs.push_back(details);
				}

				spawned_vehs = temp_vehs;
			});
		}

		components::button("Delete All", [] {
			for (auto& [_, veh] : self::spawned_vehicles)
				entity::delete_entity(veh);
			self::spawned_vehicles.clear();
		});
		ImGui::SameLine();
		ImGui::Text(std::to_string(self::spawned_vehicles.size()).c_str());

		ImGui::BeginChild("ScrollingRegion", {400, 300});

		for (int i = 0; i < spawned_vehs.size(); ++i)
		{
			auto& spawned_veh = spawned_vehs[i];
			auto veh          = spawned_veh.veh;

			ImGui::Text(spawned_veh.player_name);
			ImGui::SameLine();
			ImGui::Text(spawned_veh.veh_name);
			ImGui::SameLine();

			ImGui::PushID(i);

			components::button("TP", [=] {
				teleport::into_vehicle(veh);
			});
			ImGui::SameLine();
			components::button("Empty", [=] {
				vehicle::clear_all_peds(veh);
			});
			ImGui::SameLine();
			components::button("Delete", [=] {
				auto ent = static_cast<Entity>(veh);
				entity::delete_entity(ent);
				self::spawned_vehicles.erase(NETWORK::VEH_TO_NET(veh));
			});
			ImGui::Spacing();

			ImGui::PopID();
		}

		ImGui::EndChild();
	}
}
