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
	}
}
