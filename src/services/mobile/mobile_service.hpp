#pragma once
#include "pointers.hpp"
#include "script_global.hpp"
#include "services/players/player_service.hpp"

namespace big
{
	class personal_vehicle final
	{
		Hash m_hash;
		int m_id;
		std::string m_name;
		const char* m_plate;

		script_global m_vehicle_idx;

	public:
		personal_vehicle(int idx, script_global vehicle_idx);

		[[nodiscard]] std::string get_display_name() const;
		[[nodiscard]] Hash get_hash() const;
		[[nodiscard]] int get_id() const;
		[[nodiscard]] const char* get_plate() const;
		[[nodiscard]] script_global get_vehicle_idx() const;
		[[nodiscard]] bool is_blacklisted_vehicle() const;

		void summon() const;
	};

	class mobile_service final
	{
		std::map<std::string, std::unique_ptr<personal_vehicle>> m_personal_vehicles;

	public:
		mobile_service();
		~mobile_service();

		mobile_service(const mobile_service&)                = delete;
		mobile_service(mobile_service&&) noexcept            = delete;
		mobile_service& operator=(const mobile_service&)     = delete;
		mobile_service& operator=(mobile_service&&) noexcept = delete;

		std::map<std::string, std::unique_ptr<personal_vehicle>>& personal_vehicles()
		{
			return m_personal_vehicles;
		}
		void register_vehicles();
	};

	inline mobile_service* g_mobile_service{};
}