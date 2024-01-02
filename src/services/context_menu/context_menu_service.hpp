#pragma once
#include "gta/enums.hpp"
#include "imgui.h"
#include "natives.hpp"
#include "services/notifications/notification_service.hpp"
#include "services/vehicle/persist_car_service.hpp"
#include "util/ped.hpp"
#include "util/teleport.hpp"
#include "util/vehicle.hpp"
#include "util/delete_entity.hpp"

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
			         vehicle::lockUnlockVehicle(m_handle);
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
		    {{"TP TOP",
		         [this] {
			         teleport::tp_on_top(m_handle);
		         }},
		        {"DELETE", [this] {
			         if (ENTITY::IS_ENTITY_A_PED(m_handle))
			         {
				         g_notification_service->push_warning("Failed", "Cannot delete a ped");
				         return;
			         }
					 
			         entity::delete_entity(m_handle);
		         }}}};

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
