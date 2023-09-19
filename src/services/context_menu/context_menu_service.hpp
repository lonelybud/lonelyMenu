#pragma once
#include "backend/command.hpp"
#include "backend/player_command.hpp"
#include "natives.hpp"
#include "services/gta_data/gta_data_service.hpp"
#include "services/ped_animations/ped_animations_service.hpp"
#include "services/vehicle/persist_car_service.hpp"
#include "util/entity.hpp"
#include "util/ped.hpp"
#include "util/teleport.hpp"


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

	inline Entity lifted_vehicle;
	inline bool isVehicleLifted = false;

	inline bool ejectPlayerFromCar(Entity m_handle, int index)
	{
		bool isPlayer = ped::get_player_from_ped(VEHICLE::GET_PED_IN_VEHICLE_SEAT(m_handle, index, 0)) != NULL;

		if (isPlayer)
		{
			static player_command* command = dynamic_cast<player_command*>(command::get(rage::consteval_joaat("vehkick")));
			command->call(ped::get_player_from_ped(VEHICLE::GET_PED_IN_VEHICLE_SEAT(m_handle, index, 0)), {});
		}

		return isPlayer;
	}

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
			{"KILL ENGINE",
		         [this] {
			         if (entity::take_control_of(m_handle))
			         {
				         VEHICLE::SET_VEHICLE_ENGINE_HEALTH(m_handle, 0.f);
				         VEHICLE::SET_VEHICLE_ENGINE_ON(m_handle, false, true, false);
			         }
			         else
				         g_notification_service->push_warning("Toxic", "Failed to take control of vehicle.");
		        }},
		        {"FIX VEHICLE",
		           [this] {
			            if (entity::take_control_of(m_handle))
		        	    {
			        	    VEHICLE::SET_VEHICLE_ENGINE_HEALTH(m_handle, 1000.f);
				            VEHICLE::SET_VEHICLE_FIXED(m_handle);
				            VEHICLE::SET_VEHICLE_DEFORMATION_FIXED(m_handle);
				            VEHICLE::SET_VEHICLE_DIRT_LEVEL(m_handle, 0.f);
			            }
			            else
				            g_notification_service->push_warning("Warning", "Failed to take control of vehicle.");
		        }},		     
		        {"BURST TIRES",
		            [this] {
			            if (entity::take_control_of(m_handle))
			            {
				            VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(m_handle, true);

				            for (int i = 0; i < 8; i++)
				            {
					            VEHICLE::SET_VEHICLE_TYRE_BURST(m_handle, i, true, 1000.0);
				            }
			            }
			            else
				            g_notification_service->push_warning("Toxic", "Failed to take control of vehicle.");
		        }},
		        {"HALT",
		            [this] {
			            if (entity::take_control_of(m_handle))
			            {
				            VEHICLE::BRING_VEHICLE_TO_HALT(m_handle, 1, 5, true);
			            }
			            else
				            g_notification_service->push_warning("Toxic", "Failed to take control of vehicle.");
		        }},
		        {"COPY VEHICLE",
		            [this] {
			            Vehicle v = persist_car_service::clone_ped_car(PLAYER::PLAYER_PED_ID(), m_handle);
			            script::get_current()->yield();
			            PED::SET_PED_INTO_VEHICLE(PLAYER::PLAYER_PED_ID(), v, -1);
		        }},
		        {"BOOST",
		            [this] {
			            if (entity::take_control_of(m_handle))
				            VEHICLE::SET_VEHICLE_FORWARD_SPEED(m_handle, 79);
			            else
				            g_notification_service->push_warning("Toxic", "Failed to take control of vehicle.");
		        }},
		        {"LAUNCH",
		            [this] {
			            if (entity::take_control_of(m_handle))
				            ENTITY::APPLY_FORCE_TO_ENTITY(m_handle, 1, 0.f, 0.f, 50000.f, 0.f, 0.f, 0.f, 0, 0, 1, 1, 0, 1);
			            else
				            g_notification_service->push_warning("Toxic", "Failed to take control of vehicle.");
		        }},
		        {"EJECT",
		            [this] {
			            ejectPlayerFromCar(m_handle, -1);

			            TASK::CLEAR_PED_TASKS_IMMEDIATELY(VEHICLE::GET_PED_IN_VEHICLE_SEAT(m_handle, -1, 0));
			            TASK::CLEAR_PED_TASKS_IMMEDIATELY(m_handle);
		        }},
		        {"TP INTO", [this] {
			         teleport::into_vehicle(m_handle);
		             }},
		        {"LIFT UP", [this] {
			         if (ENTITY::IS_ENTITY_ATTACHED(m_handle) && entity::take_control_of(m_handle))
			         {
				         ENTITY::DETACH_ENTITY(m_handle, 0, 0);
			         }
			         else if (!big::isVehicleLifted)
			         {
				         int passengers = VEHICLE::GET_VEHICLE_NUMBER_OF_PASSENGERS(m_handle, 1, 0);

				         if (passengers && ejectPlayerFromCar(m_handle, -1)) // if passengers are there and driver is player then eject driver
				         {
					         int playersCount = 1;

					         if (passengers > 1) // if passengers > 1, check if other passengers are players and eject them too
						         for (int i = 0; i < VEHICLE::GET_VEHICLE_MAX_NUMBER_OF_PASSENGERS(m_handle); ++i) // get max passengers capacity except driver
							         if (ejectPlayerFromCar(m_handle, i))
								         ++playersCount;

					         int passengersWithoutPlayers = passengers - playersCount;

					         for (int i = 0; i < 100 && ((passengers = VEHICLE::GET_VEHICLE_NUMBER_OF_PASSENGERS(m_handle, 1, 0)) != passengersWithoutPlayers); i++)
						         script::get_current()->yield();

					         if (passengers != passengersWithoutPlayers) // if players are not out then do nothing
						         return;
				         }

				         if (entity::take_control_of(m_handle))
				         {
					         big::lifted_vehicle  = m_handle;
					         big::isVehicleLifted = true;

					         //  float h1 = ENTITY::GET_ENTITY_HEIGHT_ABOVE_GROUND(self::ped);
					         //  float h2 = ENTITY::GET_ENTITY_HEIGHT_ABOVE_GROUND(m_handle) / 2.0f;

					         Vector3 vehDimMin, vehDimMax, selfDimMin, selfDimMax;

					         // https://forums.gta5-mods.com/topic/30068/get-dimensions-of-entity-like-cars/2
					         MISC::GET_MODEL_DIMENSIONS(ENTITY::GET_ENTITY_MODEL(m_handle), &vehDimMin, &vehDimMax);
					         MISC::GET_MODEL_DIMENSIONS(g_local_player->m_model_info->m_hash, &selfDimMin, &selfDimMax);

					         Vector3 vehLfd = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(m_handle,
					             vehDimMin.x,
					             vehDimMax.y,
					             vehDimMin.z);
					         Vector3 vehLfu = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(m_handle,
					             vehDimMin.x,
					             vehDimMax.y,
					             vehDimMax.z);

					         Vector3 selfLfd = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(m_handle,
					             selfDimMin.x,
					             selfDimMax.y,
					             selfDimMin.z);
					         Vector3 selfLfu = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(m_handle,
					             selfDimMin.x,
					             selfDimMax.y,
					             selfDimMax.z);

					         float h1 = (selfLfu.z - selfLfd.z) / 2.f;
					         float h2 = (vehLfu.z - vehLfd.z) / 2.f;

					         ped::ped_play_animation(self::ped, "lamar_1_int-10", "cs_magenta_dual-10", -5.0, -5.0, -1, 1048635, 0.0, false, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0});
					         ENTITY::ATTACH_ENTITY_TO_ENTITY(m_handle, self::ped, ENTITY::GET_ENTITY_BONE_INDEX_BY_NAME(self::ped, "BONETAG_HEAD"), h2, 0, 0, 0, 90, 0, 1, 0, 1, 0, 0, 1, 0);
					         //  ENTITY::ATTACH_ENTITY_TO_ENTITY(m_handle, self::ped, ENTITY::GET_ENTITY_BONE_INDEX_BY_NAME(self::ped, "BONETAG_SPINE_ROOT"), h1 + h2 - 0.1f, 0, 0, 0, 90, 0, 1, 0, 1, 0, 0, 1, 0);
				         }
				         else
					         g_notification_service->push_warning("Toxic", "Failed to take control of vehicle.");
			         }
		         }}}};

		s_context_menu ped_menu{ContextEntityType::PED,
		    0,
		    {},
		    {
			{"DISARM",
		         [this] {
			         for (auto& [_, weapon] : g_gta_data_service->weapons())
				         WEAPON::REMOVE_WEAPON_FROM_PED(m_handle, weapon.m_hash);
		        }},
		        {"KILL",
		            [this] {
			            ped::kill_ped(m_handle);
		        }},
		        {"RAGDOLL",
		            [this] {
			            PED::SET_PED_TO_RAGDOLL(m_handle, 2000, 2000, 0, 0, 0, 0);
		        }},
		        {"ANIMATION",
		            [this] {
						if(STREAMING::DOES_ANIM_DICT_EXIST(g_ped_animation_service.current_animation.dict.data()))
							g_ped_animation_service.play_saved_ped_animation(g_ped_animation_service.current_animation, m_handle);
						else
							ped::ped_play_animation(m_handle, "mini@strip_club@private_dance@part1", "priv_dance_p1", 3.5f, -4.0f, -1, 1);
		        }},
		        {"RECRUIT", [this] {
			         TASK::CLEAR_PED_TASKS(m_handle);
			         PED::SET_PED_AS_GROUP_MEMBER(m_handle, PED::GET_PED_GROUP_INDEX(self::ped));
			         PED::SET_PED_RELATIONSHIP_GROUP_HASH(m_handle, PED::GET_PED_RELATIONSHIP_GROUP_HASH(self::ped));
			         PED::SET_PED_NEVER_LEAVES_GROUP(m_handle, true);
			         PED::SET_CAN_ATTACK_FRIENDLY(m_handle, 0, 1);
			         PED::SET_PED_COMBAT_ABILITY(m_handle, 2);
			         PED::SET_PED_CAN_TELEPORT_TO_GROUP_LEADER(m_handle, PED::GET_PED_GROUP_INDEX(self::ped), true);
			         PED::SET_PED_FLEE_ATTRIBUTES(m_handle, 512 | 1024 | 2048 | 16384 | 131072 | 262144, true);
			         PED::SET_PED_COMBAT_ATTRIBUTES(m_handle, 5, true);
			         PED::SET_PED_COMBAT_ATTRIBUTES(m_handle, 13, true);
			         PED::SET_PED_CONFIG_FLAG(m_handle, 394, true);
			         PED::SET_PED_CONFIG_FLAG(m_handle, 400, true);
			         PED::SET_PED_CONFIG_FLAG(m_handle, 134, true);
			         WEAPON::GIVE_WEAPON_TO_PED(m_handle, RAGE_JOAAT("weapon_microsmg"), 9999, false, false);
			         WEAPON::GIVE_WEAPON_TO_PED(m_handle, RAGE_JOAAT("weapon_carbinerifle"), 9999, false, true);
			         TASK::TASK_COMBAT_HATED_TARGETS_AROUND_PED(self::ped, 100, 67108864);
		        }}
		    }};

		s_context_menu object_menu{ContextEntityType::OBJECT, 0, {}, {}};

		s_context_menu player_menu{ContextEntityType::PLAYER,
		    0,
		    {},
		    {
			{"SET SELECTED",
		         [this] {
			         g_player_service->set_selected(ped::get_player_from_ped(m_handle));
		        }},
		        {"STEAL OUTFIT",
		            [this] {
			            ped::steal_outfit(m_handle);
		        }},
		        {"KICK",
		            [this] {
				    static player_command* command = dynamic_cast<player_command*>(command::get(rage::consteval_joaat("multikick")));
			            command->call(ped::get_player_from_ped(m_handle), {});
			            script::get_current()->yield(500ms);
		        }},
		        {"DISARM",
		            [this] {
			            static player_command* command = dynamic_cast<player_command*>(command::get(rage::consteval_joaat("remweaps")));
			            command->call(ped::get_player_from_ped(m_handle), {});
		        }},
		        {"RAGDOLL", [this] {
			         static player_command* command = dynamic_cast<player_command*>(command::get(rage::consteval_joaat("ragdoll")));
			         command->call(ped::get_player_from_ped(m_handle), {});
		        }}
		    }};

		s_context_menu shared_menu{ContextEntityType::SHARED,
		    0,
		    {},
		    {
		        {"COPY HASH", [this] {
			         ImGui::SetClipboardText(std::format("0x{:08X}", (rage::joaat_t)m_pointer->m_model_info->m_hash).c_str());
			         g_notification_service->push("Context Menu",
			             std::format("Copy hash 0x{:08X}", (rage::joaat_t)m_pointer->m_model_info->m_hash).c_str());
		        }},
			{"EXPLODE",
		         [this] {
			         rage::fvector3 pos = *m_pointer->m_navigation->get_position();
			         FIRE::ADD_EXPLOSION(pos.x, pos.y, pos.z, 1, 1000, 1, 0, 1, 0);
		        }},
		        {"TP TO",
		            [this] {
			            rage::fvector3 pos = *m_pointer->m_navigation->get_position();
			            teleport::to_coords({pos.x, pos.y, pos.z});
		        }},
		        {"TP ON TOP",
		            [this] {
			            teleport::tp_on_top(m_handle, true);
		        }},
		        {"BRING",
		            [this] {
			            rage::fvector3 pos = *g_local_player->m_navigation->get_position();

			            if (PED::IS_PED_A_PLAYER(m_handle))
			            {
				            if (auto plyr = g_player_service->get_by_id(NETWORK::NETWORK_GET_PLAYER_INDEX_FROM_PED(m_handle)))
				            {
					            teleport::teleport_player_to_coords(plyr, {pos.x, pos.y, pos.z});
				            }
			            }
			            else
			            {
				            if (entity::take_control_of(m_handle))
				            {
					            ENTITY::SET_ENTITY_COORDS(m_handle, pos.x, pos.y, pos.z, false, false, false, false);
				            }
			            }
		        }},
		        {"ENFLAME",
		            [this] {
			            Vector3 pos = ENTITY::GET_ENTITY_COORDS(m_handle, TRUE);
			            FIRE::START_ENTITY_FIRE(m_handle);
			            FIRE::START_SCRIPT_FIRE(pos.x, pos.y, pos.z, 25, TRUE);
			            FIRE::ADD_EXPLOSION(pos.x, pos.y, pos.z, eExplosionTag::MOLOTOV, 1, false, false, 0, false);
		        }},
		        {"DELETE",
		            [this] {
			            if (entity::take_control_of(m_handle))
			            {
				            entity::delete_entity(m_handle);
			            }
		        }}
		    }};

		std::unordered_map<ContextEntityType, s_context_menu> options = {{ContextEntityType::VEHICLE, vehicle_menu}, {ContextEntityType::PLAYER, player_menu}, {ContextEntityType::PED, ped_menu}, {ContextEntityType::SHARED, shared_menu}, {ContextEntityType::OBJECT, object_menu}};
	};

	inline context_menu_service* g_context_menu_service{};
}
