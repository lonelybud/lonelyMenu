#pragma once
#include "gta/enums.hpp"
#include "imgui.h"
#include "natives.hpp"
#include "services/notifications/notification_service.hpp"
#include "services/vehicle/persist_car_service.hpp"
#include "util/delete_entity.hpp"
#include "util/ped.hpp"
#include "util/teleport.hpp"
#include "util/vehicle.hpp"

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
		    {
		        {"LOCK / UNLOCK",
		            [this] {
			            vehicle::lockUnlockVehicle(m_handle);
		            }},
		        {"COPY",
		            [this] {
			            persist_car_service::clone_ped_car(m_handle);
		            }},
		        {"SAVE",
		            [this] {
			            persist_car_service::save_vehicle(m_handle, "", "");
		            }},
		        {"TP IN",
		            [this] {
			            teleport::into_vehicle(m_handle);
		            }},
		    }};

		s_context_menu player_menu{ContextEntityType::PLAYER, 0, {}, {}};

		s_context_menu ped_menu{ContextEntityType::PED,
		    0,
		    {},
		    {
		        {"KILL",
		            [this] {
			            ped::kill_ped(m_handle);
		            }},
		        {"RECRUIT",
		            [this] {
			            auto group_id = PED::GET_PED_GROUP_INDEX(self::ped);

			            TASK::CLEAR_PED_TASKS(m_handle);
			            if (!PED::IS_PED_GROUP_MEMBER(m_handle, group_id))
			            {
				            PED::SET_PED_AS_GROUP_MEMBER(m_handle, group_id);
				            PED::SET_PED_RELATIONSHIP_GROUP_HASH(m_handle, PED::GET_PED_RELATIONSHIP_GROUP_HASH(self::ped));
				            PED::SET_PED_NEVER_LEAVES_GROUP(m_handle, 1);
				            PED::SET_CAN_ATTACK_FRIENDLY(m_handle, 0, 1);
				            PED::SET_PED_COMBAT_ABILITY(m_handle, 2);
				            PED::SET_PED_CAN_TELEPORT_TO_GROUP_LEADER(m_handle, group_id, 1);
				            PED::SET_PED_FLEE_ATTRIBUTES(m_handle, 512 | 1024 | 2048 | 16384 | 131072 | 262144, 1);
				            PED::SET_PED_COMBAT_ATTRIBUTES(m_handle, 5, 1);
				            PED::SET_PED_COMBAT_ATTRIBUTES(m_handle, 13, 1);
				            PED::SET_PED_CONFIG_FLAG(m_handle, 394, 1);
				            PED::SET_PED_CONFIG_FLAG(m_handle, 400, 1);
				            PED::SET_PED_CONFIG_FLAG(m_handle, 134, 1);
			            }
			            else
				            PED::REMOVE_PED_FROM_GROUP(m_handle);
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
		        {"DELETE",
		            [this] {
			            if (ENTITY::IS_ENTITY_A_PED(m_handle))
			            {
				            g_notification_service->push_warning("Failed", "Cannot delete a ped");
				            return;
			            }

			            entity::delete_entity(m_handle);
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
