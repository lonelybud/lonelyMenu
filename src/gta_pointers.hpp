#pragma once
#include "function_types.hpp"

#include <memory/handle.hpp>

class CCommunications;
class FriendRegistry;
class CNetworkPlayerMgr;
class Network;
class ChatData;
class ScInfo;
class GenericPool;
class VehiclePool;
class CVehicleSeatMetadataMgr;
class CVehicleDriveByMetadataMgr;
class CBlipList;
class TimecycleKeyframeData;
class CTrainConfig;
class CWeaponInfoManager;
class CPedFactory;
class GtaThread;

namespace rage
{
	template<typename T>
	class atSingleton;

	template<typename T>
	class atArray;

	class RageSecurity;
	class netTime;
	class rlGamerInfo;
	struct game_skeleton;
	class scrProgramTable;
}

template<typename T>
class HashTable;

namespace big
{
	// needed for serialization of the pointers cache
#pragma pack(push, 1)
	struct gta_pointers
	{
		memory::handle m_creator_warp_cheat_triggered_patch;

		memory::handle m_sound_overload_detour;

		memory::handle m_crash_trigger;

		memory::handle m_script_vm_patch_1;
		memory::handle m_script_vm_patch_2;
		memory::handle m_script_vm_patch_3;
		memory::handle m_script_vm_patch_4;
		memory::handle m_script_vm_patch_5;
		memory::handle m_script_vm_patch_6;

		bool* m_is_session_started;

		CPedFactory** m_ped_factory;
		CNetworkPlayerMgr** m_network_player_mgr;
		CNetworkObjectMgr** m_network_object_mgr;
		rage::game_skeleton* m_game_skeleton;
		void (*m_nullsub)();

		functions::ptr_to_handle m_ptr_to_handle;
		functions::handle_to_ptr m_handle_to_ptr;
		rage::scrNativeRegistrationTable* m_native_registration_table;
		functions::get_native_handler m_get_native_handler;
		functions::fix_vectors m_fix_vectors;

		rage::atArray<GtaThread*>* m_script_threads;
		rage::scrProgramTable* m_script_program_table;
		functions::run_script_threads m_run_script_threads;
		std::int64_t** m_script_globals;

		IDXGISwapChain** m_swapchain;

		int* m_resolution_x;
		int* m_resolution_y;

		PVOID m_get_label_text;

		FriendRegistry* m_friend_registry;

		functions::get_screen_coords_for_world_coords m_get_screen_coords_for_world_coords;

		HashTable<CBaseModelInfo*>* m_model_table;

		functions::trigger_script_event m_trigger_script_event;

		PVOID m_network_player_mgr_init;
		PVOID m_network_player_mgr_shutdown;

		functions::get_gameplay_cam_coords m_get_gameplay_cam_coords;

		// Bitbuffer Read/Write START
		functions::read_bitbuf_dword m_read_bitbuf_dword;
		functions::read_bitbuf_string m_read_bitbuf_string;
		functions::read_bitbuf_bool m_read_bitbuf_bool;
		functions::read_bitbuf_array m_read_bitbuf_array;
		functions::write_bitbuf_qword m_write_bitbuf_qword;
		functions::write_bitbuf_dword m_write_bitbuf_dword;
		functions::write_bitbuf_int64 m_write_bitbuf_int64;
		functions::write_bitbuf_int32 m_write_bitbuf_int32;
		functions::write_bitbuf_bool m_write_bitbuf_bool;
		functions::write_bitbuf_array m_write_bitbuf_array;
		// Bitbuffer Read/Write END

		// Received Event Signatures START
		PVOID m_received_event;
		functions::send_event_ack m_send_event_ack;
		// Received Event Signatures END

		//Sync Signatures START
		PVOID m_received_clone_create;
		PVOID m_received_clone_sync;
		PVOID m_can_apply_data;
		functions::get_sync_tree_for_type m_get_sync_tree_for_type;
		functions::get_net_object m_get_net_object;
		//Sync Signatures END

		PVOID m_receive_net_message;
		PVOID m_get_network_event_data;
		PVOID m_assign_physical_index;

		Network** m_network;

		functions::reset_network_complaints m_reset_network_complaints;

		functions::fidevice_get_device m_fidevice_get_device;
		functions::fipackfile_ctor m_fipackfile_ctor;
		functions::fipackfile_dtor m_fipackfile_dtor;
		rage::fiPackfile** m_fipackfile_instances;
		functions::fipackfile_open_archive m_fipackfile_open_archive;
		functions::fipackfile_mount m_fipackfile_mount;
		functions::fipackfile_unmount m_fipackfile_unmount;
		functions::fipackfile_close_archive m_fipackfile_close_archive;

		PVOID m_invalid_decal_crash;
		PVOID m_task_parachute_object;
		PVOID m_task_ambient_clips;

		int64_t** m_send_chat_ptr;
		functions::send_chat_message m_send_chat_message;

		PVOID m_init_native_tables;
		functions::script_vm m_script_vm;

		uint64_t* m_host_token;
		rage::rlGamerInfo* m_profile_gamer_info;     // per profile gamer info
		rage::rlGamerInfo* m_player_info_gamer_info; // the gamer info that is applied to CPlayerInfo
		CCommunications** m_communications;

		PVOID m_handle_join_request;
		functions::write_join_response_data m_write_join_response_data;

		functions::queue_packet m_queue_packet;

		PVOID m_serialize_join_request_message;
		PVOID m_serialize_join_request_message_2;

		functions::request_control m_request_control;

		functions::get_connection_peer m_get_connection_peer;
		functions::handle_remove_gamer_cmd m_handle_remove_gamer_cmd;

		PVOID m_prepare_metric_for_sending;

		PVOID m_queue_dependency;

		PVOID m_http_start_request;

		PVOID m_serialize_take_off_ped_variation_task;
		PVOID m_serialize_parachute_task;

		functions::send_packet m_send_packet;

		PVOID m_fragment_physics_crash_2;

		PVOID m_received_array_update;

		PVOID m_receive_pickup;

		const char* m_game_version;
		const char* m_online_version;

		GenericPool** m_ped_pool;
		GenericPool** m_prop_pool;
		VehiclePool*** m_vehicle_pool;

		PVOID m_model_spawn_bypass;

		functions::get_host_array_handler_by_index m_get_host_array_handler_by_index;

		CBlipList* m_blip_list;
		
		PVOID m_allocate_memory_reliable;
		functions::connection_manager_try_free_memory m_connection_manager_try_free_memory;
		functions::remove_message_from_queue m_remove_message_from_queue;
		functions::remove_message_from_unacked_reliables m_remove_message_from_unacked_reliables;

		PVOID m_free_event_error;

		PVOID* m_draw_handler_mgr;
		PVOID m_render_ped;
		PVOID m_render_entity;
		PVOID m_render_big_ped;

		functions::remove_reference m_remove_reference;

		void** m_sync_data_reader_vtable;

		GenericPool** m_interior_proxy_pool;

		rage::atArray<CTrainConfig>* m_train_config_array;

		PVOID m_activate_special_ability_patch;

		functions::get_ped_seat m_get_ped_seat;

		functions::received_clone_remove m_received_clone_remove;

		CWeaponInfoManager* m_weapon_info_manager;

		PVOID m_searchlight_crash;
		functions::get_searchlight m_get_searchlight;

		GenericPool** m_vehicle_allocator; // this is not a normal pool

		PVOID m_write_node_data;
	};
#pragma pack(pop)
	static_assert(sizeof(gta_pointers) % 8 == 0, "Pointers are not properly aligned");
}