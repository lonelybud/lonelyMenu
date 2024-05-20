#pragma once
#include <network/CNetGamePlayer.hpp>
#include <rage/rlTaskStatus.hpp>
#include <script/scrNativeHandler.hpp>
#include <script/scrProgram.hpp>

class CMsgJoinResponse;
class NetworkGameFilterMatchmakingComponent;
class sCloudFile;
class CPlayerGameStateDataNode;
class CVehicleGadgetDataNode;
class CGameScriptHandlerNetComponent;
class CDoorBreakEvent;
class GenericPool;
class CGetPedSeatReturnClass;
enum eVehicleGadgetType : uint32_t;
enum class PedBones : uint16_t;
class CNetComplaintMgr;
class Network;
class CNetworkObjectMgr;

namespace rage
{
	class netConnectionManager;
	class netPeerAddress;
	class netConnection;
	class netMessageQueue;
	class netQueuedMessage;
	class netConnectionPeer;
	class snMsgRemoveGamersFromSessionCmd;
	class snSession;
	class snPlayer;
	class CDynamicEntity;
	class netTimeSyncMsg;
	class snConnectToPeerTaskData;
	class snConnectToPeerTaskResult;
	class rlScHandle;
	class rlQueryPresenceAttributesContext;
	enum class eThreadState : uint32_t;
	class netArrayHandlerBase;
	class fwRefAwareBase;
	class fwEntity;
	class netGameEvent;
	class netEventMgr;
	class fiDevice;
	class fiPackfile;
	class scrNativeRegistrationTable;
	class rlSessionByGamerTaskResult;
	struct rlScTaskStatus
	{
		void* pad  = 0;
		int status = 0;
		int unk    = 0;
	};
}

namespace datafile_commands
{
	class SveFileObject;
}

namespace big::functions
{
	using run_script_threads = bool (*)(uint32_t ops_to_execute);
	using get_native_handler = rage::scrNativeHandler (*)(rage::scrNativeRegistrationTable* registration_table, rage::scrNativeHash hash);
	using fix_vectors = void (*)(rage::scrNativeCallContext* call_ctx);

	using get_net_game_player = CNetGamePlayer* (*)(Player player);

	using trigger_script_event = void (*)(int event_group, int64_t* args, int arg_count, int player_bits, int event_id);

	using increment_stat_event = bool (*)(uint64_t net_event_struct, int64_t sender, int64_t a3);

	using ptr_to_handle = Entity (*)(void*);
	using handle_to_ptr = rage::CDynamicEntity* (*)(Entity);

	using write_player_game_state_data_node = bool (*)(rage::netObject* plr, CPlayerGameStateDataNode* node);

	using get_gameplay_cam_coords = Vector3 (*)();

	using get_screen_coords_for_world_coords = bool (*)(float* world_coords, float* out_x, float* out_y);

	// Bitbuffer read/write START
	using read_bitbuf_dword  = bool (*)(rage::datBitBuffer* buffer, PVOID read, int bits);
	using read_bitbuf_string = bool (*)(rage::datBitBuffer* buffer, char* read, int bits);
	using read_bitbuf_bool   = bool (*)(rage::datBitBuffer* buffer, bool* read, int bits);
	using read_bitbuf_array  = bool (*)(rage::datBitBuffer* buffer, PVOID read, int bits, int unk);
	using write_bitbuf_qword = bool (*)(rage::datBitBuffer* buffer, uint64_t val, int bits);
	using write_bitbuf_dword = bool (*)(rage::datBitBuffer* buffer, uint32_t val, int bits);
	using write_bitbuf_int64 = bool (*)(rage::datBitBuffer* buffer, int64_t val, int bits);
	using write_bitbuf_int32 = bool (*)(rage::datBitBuffer* buffer, int32_t val, int bits);
	using write_bitbuf_bool  = bool (*)(rage::datBitBuffer* buffer, bool val, int bits);
	using write_bitbuf_array = bool (*)(rage::datBitBuffer* buffer, void* val, int bits, int unk);

	// Bitbuffer read/write END
	// Received Event Signatures START
	using send_event_ack = void (*)(rage::netEventMgr* event_manager, CNetGamePlayer* source_player, CNetGamePlayer* target_player, int event_index, int event_handled_bitset);
	// Received Event Signatures END

	//Sync signatures START
	using get_sync_tree_for_type = rage::netSyncTree* (*)(CNetworkObjectMgr* mgr, uint16_t sync_type);

	using get_net_object = rage::netObject* (*)(CNetworkObjectMgr* mgr, int16_t id, bool can_delete_be_pending);
	//Sync signatures END

	using reset_network_complaints = void (*)(CNetComplaintMgr* mgr);

	using fidevice_get_device = rage::fiDevice* (*)(const char* path, bool allow_root);
	using fipackfile_ctor     = rage::fiPackfile* (*)(rage::fiPackfile* this_);
	using fipackfile_dtor     = rage::fiPackfile* (*)(rage::fiPackfile* this_);
	using fipackfile_open_archive = bool (*)(rage::fiPackfile* this_, const char* archive, bool b_true, int type, intptr_t very_false);
	using fipackfile_mount         = bool (*)(rage::fiPackfile* this_, const char* mount_point);
	using fipackfile_unmount       = bool (*)(const char* mount_point);
	using fipackfile_close_archive = void (*)(rage::fiDevice* this_);

	using start_get_presence_attributes = bool (*)(int profile_index, rage::rlScHandle* handle, int num_handles, rage::rlQueryPresenceAttributesContext** contexts, int count, rage::rlScTaskStatus* state);

	using get_vehicle_gadget_array_size = int (*)(eVehicleGadgetType type);

	using write_join_response_data = bool (*)(CMsgJoinResponse* response, void* data, int size, uint32_t* size_used);

	using queue_packet = bool (*)(rage::netConnectionManager* mgr, int msg_id, void* data, int size, int flags, void* unk);

	using send_chat_message = bool (*)(int64_t* send_chat_ptr, rage::rlGamerInfo* gamer_info, char* message, bool is_team);

	using request_control = void (*)(rage::netObject* net_object);

	using get_connection_peer = rage::netConnectionPeer* (*)(rage::netConnectionManager* manager, int peer_id);
	using handle_remove_gamer_cmd = void* (*)(rage::snSession* session, rage::snPlayer* origin, rage::snMsgRemoveGamersFromSessionCmd* cmd);

	using script_vm = rage::eThreadState (*)(uint64_t* stack, int64_t** scr_globals, rage::scrProgram* program, rage::scrThreadContext* ctx);

	using send_packet = bool (*)(rage::netConnectionManager* mgr, rage::netPeerAddress* adde, int connection_id, void* data, int size, int flags);

	using get_host_array_handler_by_index = rage::netArrayHandlerBase* (*)(CGameScriptHandlerNetComponent* component, int index);

	using connection_manager_try_free_memory = void (*)(rage::netConnectionManager* mgr);
	using remove_message_from_queue          = void (*)(rage::netMessageQueue* queue, rage::netQueuedMessage* message);
	using remove_message_from_unacked_reliables = void (*)(void* list, uint16_t* unk);

	using remove_reference = void (*)(rage::fwRefAwareBase* object, void* reference);

	using get_ped_seat = CGetPedSeatReturnClass* (*)(PVOID seat_info, CPed* ped);
	
	using received_clone_remove = void (*)(CNetworkObjectMgr*, CNetGamePlayer*, CNetGamePlayer*, int16_t, uint32_t);

	using get_searchlight = void* (*) (CPed*);
}
