#pragma once
#include "gta/enums.hpp"
#include "imgui.h"
#include "natives.hpp"
#include "services/notifications/notification_service.hpp"
#include "services/vehicle/persist_car_service.hpp"
#include "util/entity.hpp"
#include "util/ped.hpp"
#include "util/teleport.hpp"

#include <entities/CDynamicEntity.hpp>

namespace big
{
	struct context_option
	{
		std::string name;
		std::function<void()> command;
	};

	struct s_context_menu
	{
		ContextEntityType type;
		int current_option = 0;
		ImVec2 menu_size   = {};
		std::vector<context_option> options;
	};

	struct model_bounding_box_screen_space
	{
		ImVec2 edge1, edge2, edge3, edge4;
		ImVec2 edge5, edge6, edge7, edge8;
	};

	class context_menu_service final
	{
	private:
		void fill_model_bounding_box_screen_space();
		static double distance_to_middle_of_screen(const rage::fvector2& screen_pos);

	public:
		context_menu_service();
		~context_menu_service();

		context_menu_service(const context_menu_service&)                = delete;
		context_menu_service(context_menu_service&&) noexcept            = delete;
		context_menu_service& operator=(const context_menu_service&)     = delete;
		context_menu_service& operator=(context_menu_service&&) noexcept = delete;

		bool enabled = false;
		s_context_menu* get_context_menu();
		void get_entity_closest_to_screen_center();
		void load_shared();

		static void disable_control_action_loop();
		static void context_menu();

		Entity m_handle;
		rage::fwEntity* m_pointer{};
		model_bounding_box_screen_space m_model_bounding_box_screen_space;

		s_context_menu vehicle_menu{ContextEntityType::VEHICLE,
		    0,
		    {},
		    {{"COPY",
		         [this] {
			         persist_car_service::clone_ped_car(m_handle);
		         }},
		        {"LOCK / UNLOCK", [this] {
			         if (entity::take_control_of(m_handle))
			         {
				         auto door_locked_state = (eVehicleLockState)VEHICLE::GET_VEHICLE_DOOR_LOCK_STATUS(m_handle);
				         if (door_locked_state == eVehicleLockState::VEHICLELOCK_LOCKED)
					         VEHICLE::SET_VEHICLE_DOORS_LOCKED(m_handle, (int)eVehicleLockState::VEHICLELOCK_NONE);
				         else
					         VEHICLE::SET_VEHICLE_DOORS_LOCKED(m_handle, (int)eVehicleLockState::VEHICLELOCK_LOCKED);
			         }
			         else
				         g_notification_service->push_warning("Toxic", "Failed to take control of vehicle.");
		         }}}};

		s_context_menu player_menu{ContextEntityType::PLAYER, 0, {}, {}};

		s_context_menu ped_menu{ContextEntityType::PED,
		    0,
		    {},
		    {
		        {"KILL",
		            [this] {
			            ped::kill_ped(m_handle);
		            }},
		    }};

		s_context_menu object_menu{ContextEntityType::OBJECT, 0, {}, {}};

		s_context_menu shared_menu{ContextEntityType::SHARED,
		    0,
		    {},
		    {
		        {"TP TOP",
		            [this] {
			            teleport::tp_on_top(m_handle);
		            }},
		    }};

		std::unordered_map<ContextEntityType, s_context_menu> options = {
		    {ContextEntityType::VEHICLE, vehicle_menu},
		    {ContextEntityType::PLAYER, player_menu},
		    {ContextEntityType::PED, ped_menu},
		    {ContextEntityType::SHARED, shared_menu},
		    {ContextEntityType::OBJECT, object_menu},
		};
	};

	inline context_menu_service* g_context_menu_service{};
}
