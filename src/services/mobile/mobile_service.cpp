#include "mobile_service.hpp"

#include "gta/joaat.hpp"
#include "natives.hpp"
#include "script.hpp"
#include "util/mobile.hpp"

namespace big
{
	personal_vehicle::personal_vehicle(int idx, script_global vehicle_idx) :
	    m_id(idx),
	    m_vehicle_idx(vehicle_idx)
	{
		m_plate = m_vehicle_idx.at(1).as<char*>();
		m_hash  = *m_vehicle_idx.at(66).as<Hash*>();
		
		m_name = std::format("{} ({})", HUD::GET_FILENAME_FOR_AUDIO_CONVERSATION(VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(m_hash)), m_plate);
		std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
	}

	std::string personal_vehicle::get_display_name() const
	{
		return m_name + "##" + std::to_string(m_id);
	}

	Hash personal_vehicle::get_hash() const
	{
		return m_hash;
	}

	int personal_vehicle::get_id() const
	{
		return m_id;
	}

	const char* personal_vehicle::get_plate() const
	{
		return m_plate;
	}

	script_global personal_vehicle::get_vehicle_idx() const
	{
		return m_vehicle_idx;
	}

	void personal_vehicle::summon() const
	{
		mobile::mechanic::summon_vehicle_by_index(m_id);
	}

	bool personal_vehicle::is_blacklisted_vehicle() const
	{
		switch (m_hash)
		{
		case RAGE_JOAAT("avenger"):
		case RAGE_JOAAT("avenger3"):
		case RAGE_JOAAT("hauler2"):
		case RAGE_JOAAT("phantom3"):
		case RAGE_JOAAT("trailersmall2"):
		case RAGE_JOAAT("chernobog"):
		case RAGE_JOAAT("riot2"):
		case RAGE_JOAAT("thruster"):
		case RAGE_JOAAT("brickade2"):
		case RAGE_JOAAT("manchez3"):
		case RAGE_JOAAT("terbyte"):
		case RAGE_JOAAT("speedo4"):
		case RAGE_JOAAT("mule4"):
		case RAGE_JOAAT("pounder2"):
		case RAGE_JOAAT("rcbandito"):
		case RAGE_JOAAT("minitank"): return true;
		}
		return false;
	}

	//

	mobile_service::mobile_service()
	{
		g_mobile_service = this;
	}

	mobile_service::~mobile_service()
	{
		g_mobile_service = nullptr;
	}

	void mobile_service::register_vehicles()
	{
		personal_vehicles.clear();

		for (int i = 0; i < *scr_globals::vehicle_global.as<int*>(); ++i)
		{
			auto veh_idx_global = scr_globals::vehicle_global.at(i, 142);
			const auto hash     = *veh_idx_global.at(66).as<Hash*>();

			// double check if model is a vehicle
			if (STREAMING::IS_MODEL_A_VEHICLE(hash))
				if (auto veh = std::make_unique<personal_vehicle>(i, veh_idx_global); !veh->is_blacklisted_vehicle())
					personal_vehicles.emplace(veh->get_display_name(), std::move(veh));
		}
	}
}