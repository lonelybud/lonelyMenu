#include "core/data/vehicle.hpp"
#include "natives.hpp"
#include "services/notifications/notification.hpp"
#include "util/delete_entity.hpp"
#include "util/teleport.hpp"
#include "views/view.hpp"

namespace big
{
	static int tick_count = 60;
	static bool deleting  = false;

	void view::spawned_vehicles()
	{
		++tick_count;

		if (tick_count > 60)
		{
			tick_count = 0;
			g_fiber_pool->queue_job([] {
				for (auto& pair : g_vehicle.spawned_vehicles)
				{
					auto veh = HUD::GET_BLIP_INFO_ID_ENTITY_INDEX(pair.second.blip);

					if (ENTITY::DOES_ENTITY_EXIST(veh) && !ENTITY::IS_ENTITY_DEAD(veh, 0))
					{
						pair.second.passenger_count = VEHICLE::GET_VEHICLE_NUMBER_OF_PASSENGERS(veh, 1, 0);
						pair.second.is_locked = (eVehicleLockState)VEHICLE::GET_VEHICLE_DOOR_LOCK_STATUS(veh) == eVehicleLockState::VEHICLELOCK_LOCKED;
					}
					else
						pair.second.is_dead = true;
				}
			});
		}

		components::button("Delete All", [] {
			if (deleting)
				return g_notification_service.push_error("Failed", "Delete already in process");

			deleting = true;

			auto temp = g_vehicle.spawned_vehicles;
			for (auto it = g_vehicle.spawned_vehicles.begin(); it != g_vehicle.spawned_vehicles.end(); ++it)
				if (auto ent = HUD::GET_BLIP_INFO_ID_ENTITY_INDEX(it->second.blip); entity::delete_entity(ent))
					if (auto itr = temp.find(it->first); itr != temp.end())
						temp.erase(itr);

			g_vehicle.spawned_vehicles = temp;

			deleting = false;
		});
		ImGui::SameLine();
		ImGui::Text(std::to_string(g_vehicle.spawned_vehicles.size()).c_str());

		ImGui::BeginChild("ScrollingRegion", {400, 300});
		for (auto it = g_vehicle.spawned_vehicles.begin(); it != g_vehicle.spawned_vehicles.end(); ++it)
		{
			ImGui::PushID(it->first);

			ImGui::Text(it->second.vehicle_name.c_str());

			if (it->second.is_dead)
				ImGui::Text("DEAD");
			else
			{
				ImGui::Text("Passengers: %d, %s", it->second.passenger_count, it->second.is_locked ? "LOCKED" : "FREE");

				components::button("TP", [it] {
					auto veh         = HUD::GET_BLIP_INFO_ID_ENTITY_INDEX(it->second.blip);
					Vector3 location = ENTITY::GET_ENTITY_COORDS(veh, 0);
					teleport::to_coords(location);
				});
				ImGui::SameLine();
				components::button("Waypoint", [it] {
					auto veh         = HUD::GET_BLIP_INFO_ID_ENTITY_INDEX(it->second.blip);
					Vector3 location = ENTITY::GET_ENTITY_COORDS(veh, 0);
					HUD::SET_NEW_WAYPOINT(location.x, location.y);
				});
				// ImGui::SameLine();
				// components::button("Blip", [it] {
				// 	auto veh = HUD::GET_BLIP_INFO_ID_ENTITY_INDEX(it->second.blip);
				// 	if (!it->second.blip)
				// 		vehicle::add_blip_to_spawned_veh(veh, it->second);
				// 	else
				// 	{
				// 		HUD::REMOVE_BLIP(&it->second.blip);
				// 		it->second.blip = 0;
				// 	}
				// });
				ImGui::SameLine();
				components::button("Delete", [it] {
					if (deleting)
						return g_notification_service.push_error("Failed", "Delete already in process");

					deleting = true;

					auto ent = HUD::GET_BLIP_INFO_ID_ENTITY_INDEX(it->second.blip);
					if (entity::delete_entity(ent))
						g_vehicle.spawned_vehicles.erase(it);

					deleting = false;
				});
			}

			ImGui::PopID();

			ImGui::Spacing();
		}
		ImGui::EndChild();
	}
}
