#include "core/data/vehicle.hpp"
#include "natives.hpp"
#include "util/delete_entity.hpp"
#include "util/teleport.hpp"
#include "views/view.hpp"

namespace big
{
	static int tick_count = 60;

	void view::spawned_vehicles()
	{
		++tick_count;

		if (tick_count > 60)
		{
			tick_count = 0;
			g_fiber_pool->queue_job([] {
				for (auto& pair : g_vehicle.spawned_vehicles)
					if (ENTITY::DOES_ENTITY_EXIST(pair.first) && !ENTITY::IS_ENTITY_DEAD(pair.first, 0))
					{
						pair.second.passenger_count = VEHICLE::GET_VEHICLE_NUMBER_OF_PASSENGERS(pair.first, 1, 0);
						pair.second.is_locked = (eVehicleLockState)VEHICLE::GET_VEHICLE_DOOR_LOCK_STATUS(pair.first) == eVehicleLockState::VEHICLELOCK_LOCKED;
					}
					else
						pair.second.is_dead = true;
			});
		}

		components::button("Delete All", [] {
			auto temp = g_vehicle.spawned_vehicles;
			for (auto it = g_vehicle.spawned_vehicles.begin(); it != g_vehicle.spawned_vehicles.end(); ++it)
			{
				auto ent = it->first;
				if (entity::delete_entity(ent))
					if (auto itr = temp.find(it->first); itr != temp.end())
						temp.erase(itr);
			}
			g_vehicle.spawned_vehicles = temp;
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
				ImGui::Text(std::format("Passengers: {}, {}", it->second.passenger_count, it->second.is_locked ? "LOCKED" : "FREE")
				                .c_str());

				components::button("TP", [it] {
					Vector3 location = ENTITY::GET_ENTITY_COORDS(it->first, 0);
					teleport::to_coords(location);
				});
				ImGui::SameLine();
				components::button("Waypoint", [it] {
					Vector3 location = ENTITY::GET_ENTITY_COORDS(it->first, 0);
					HUD::SET_NEW_WAYPOINT(location.x, location.y);
				});
				ImGui::SameLine();
				components::button("Delete", [it] {
					auto ent = static_cast<Entity>(it->first);
					if (entity::delete_entity(ent))
						g_vehicle.spawned_vehicles.erase(it);
				});
			}

			ImGui::PopID();

			ImGui::Spacing();
		}
		ImGui::EndChild();
	}
}
