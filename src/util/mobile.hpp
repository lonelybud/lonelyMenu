#pragma once
#include "core/enums.hpp"
#include "core/scr_globals.hpp"
#include "globals.hpp"
#include "gta_util.hpp"
#include "misc.hpp"
#include "natives.hpp"
#include "notify.hpp"
#include "script.hpp"
#include "script_local.hpp"
#include "services/notifications/notification_service.hpp"
#include "util/mobile.hpp"
#include "vehicle.hpp"

namespace big::mobile
{
	namespace mors_mutual
	{
		inline bool fix_index(int veh_idx, bool spawn_veh = false)
		{
			bool can_be_fixed = misc::has_bits_set(scr_globals::vehicle_global.at(veh_idx, 142).at(103).as<int*>(), eVehicleFlags::DESTROYED | eVehicleFlags::HAS_INSURANCE);

			if (can_be_fixed)
				misc::clear_bits(scr_globals::vehicle_global.at(veh_idx, 142).at(103).as<int*>(), eVehicleFlags::DESTROYED | eVehicleFlags::IMPOUNDED | eVehicleFlags::UNK2);

			return can_be_fixed;
		}

		inline int fix_all()
		{
			int fixed_count = 0;

			const int arr_size = *scr_globals::vehicle_global.as<int*>();
			for (int i = 0; i < arr_size; i++)
				if (fix_index(i, true))
					fixed_count++;

			return fixed_count;
		}
	}
	namespace mechanic
	{
		inline Vehicle get_personal_vehicle()
		{
			return *scr_globals::freemode_global.at(299).as<Vehicle*>();
		}

		inline void summon_vehicle_by_index(int veh_idx)
		{
			if (*scr_globals::freemode_global.at(985).as<int*>() != -1)
			{
				g_notification_service->push_warning("Vehicle", "Mechanic is not ready to deliver a vehicle right now.");
				return;
			}

			if (auto veh = get_personal_vehicle(); (veh && ENTITY::IS_ENTITY_A_VEHICLE(veh)))
			{
				g_notification_service->push_warning("Vehicle", "PV already exists.");
				return;
			}

			script::get_current()->yield(100ms);

			*scr_globals::freemode_global.at(928).as<int*>() = 1; // tell freemode to spawn our vehicle
			*scr_globals::freemode_global.at(988).as<int*>() = 0; // required
			*scr_globals::freemode_global.at(985).as<int*>() = veh_idx;

			script::get_current()->yield(100ms);

			// blocking call till vehicle is delivered
			int *address = scr_globals::freemode_global.at(985).as<int*>(), value = -1;
			for (size_t i = 0; *address != value && i < 100; i++)
				script::get_current()->yield(100ms);
		}
	}
}
