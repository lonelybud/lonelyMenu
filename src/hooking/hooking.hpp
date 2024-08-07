#pragma once
#include "MinHook.h"
#include "call_hook.hpp"
#include "detour_hook.hpp"
#include "gta/enums.hpp"
#include "vmt_hook.hpp"
#include "vtable_hook.hpp"

#include <network/CNetGamePlayer.hpp>
#include <network/netConnection.hpp> // cannot stub this
#include <script/scrProgram.hpp>

class CPlayerGamerDataNode;
class CPlayerGameStateDataNode;
class CPedInventoryDataNode;
class CDynamicEntityGameStateDataNode;
class CVehicleGadgetDataNode;
class CPlayerCameraDataNode;
class CJoinRequestContext;
class SessionSortEntry;
class RemoteGamerInfoMsg;
class CMsgTextMessage;
class CNetGamePlayerDataMsg;
class MatchmakingAttributes;
class CNetworkIncrementStatEvent;
class CScriptedGameEvent;
class NetworkGameFilterMatchmakingComponent;
class ClonedTakeOffPedVariationInfo;
class CPlayerCardStats;
class CStatsSerializationContext;
class CPlayerCreationDataNode;
class CPlayerAppearanceDataNode;
class CFoundDevice;
class IDirectSoundCapture;
class CVehicleProximityMigrationDataNode;
class CNonPhysicalPlayerData;
class TimecycleKeyframeData;
class CPedTaskSpecificDataNode;
class CPedTaskSequenceDataNode;
class Network;
class GtaThread;
class CNetworkPlayerMgr;
class CNetworkObjectMgr;
class CPhysicalScriptGameStateDataNode;
class MatchmakingId;
class CMsgJoinRequest;
class GenericPool;

enum class eAckCode : uint32_t;

namespace rage
{
	class snSession;
	class JSONNode;
	class netArrayHandlerBase;
	class CEventNetwork;
	class CSyncDataBase;
	class rlGamerHandle;
	class netConnectionManager;
	class datBitBuffer;
	class rlMetric;
	class rlTaskStatus;
	class netEventMgr;
	class json_serializer;
	class netGameEvent;
	class netSyncDataNode;
	class rlSessionDetailMsg;
	class netEvent;
}

namespace big
{
	struct hooks
	{
		static bool run_script_threads(uint32_t ops_to_execute);

		// gui

		static constexpr auto swapchain_num_funcs           = 19;
		static constexpr auto swapchain_present_index       = 8;
		static constexpr auto swapchain_resizebuffers_index = 13;
		static HRESULT swapchain_present(IDXGISwapChain* this_, UINT sync_interval, UINT flags);
		static HRESULT swapchain_resizebuffers(IDXGISwapChain* this_, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT swapchain_flags);
		static LRESULT wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

		// script

		static bool init_native_tables(rage::scrProgram* program);
		static rage::eThreadState script_vm(uint64_t* start_stack, uint64_t** scr_globals, rage::scrProgram* program, rage::scrThreadContext* ctx);

		// player management

		static bool network_player_mgr_init(CNetworkPlayerMgr* _this, uint64_t a2, uint32_t a3, uint32_t a4[4]);
		static void network_player_mgr_shutdown(CNetworkPlayerMgr* _this);
		static void* assign_physical_index(CNetworkPlayerMgr* netPlayerMgr, CNetGamePlayer* player, uint8_t new_index);

		// info

		static rage::CEventNetwork* get_network_event_data(int64_t unk, rage::CEventNetwork* net_event);
		static bool prepare_metric_for_sending(rage::json_serializer* bit_buffer, int unk, int time, rage::rlMetric* metric);

		// protections

		static bool fragment_physics_crash_2(float* a1, float* a2);
		static void received_event(rage::netEventMgr* event_manager, CNetGamePlayer* source_player, CNetGamePlayer* target_player, uint16_t event_id, int event_index, int event_handled_bitset, int unk, rage::datBitBuffer* bit_buffer);
		// *** these two aren't actually hooks, just helper functions for hooks
		static bool scripted_game_event(CScriptedGameEvent* scripted_game_event, CNetGamePlayer* player);
		// ***
		static bool receive_net_message(void* a1, rage::netConnectionManager* mgr, rage::netEvent* event);
		static void received_clone_create(CNetworkObjectMgr* mgr, CNetGamePlayer* src, CNetGamePlayer* dst, eNetObjType object_type, int32_t object_id, int32_t object_flag, rage::datBitBuffer* buffer, int32_t timestamp);
		static eAckCode received_clone_sync(CNetworkObjectMgr* mgr, CNetGamePlayer* src, CNetGamePlayer* dst, eNetObjType object_type, uint16_t object_id, rage::datBitBuffer* bufer, uint16_t unk, uint32_t timestamp);
		static bool can_apply_data(rage::netSyncTree* tree, rage::netObject* object);
		static void invalid_decal(uintptr_t a1, int a2);
		static int task_parachute_object(uint64_t _this, int a2, int a3);
		static int task_ambient_clips(uint64_t _this, int a2, int a3);
		static bool handle_join_request(Network* network, rage::snSession* session, rage::rlGamerInfo* player_info, CJoinRequestContext* ctx, BOOL is_transition_session);
		static void serialize_take_off_ped_variation_task(ClonedTakeOffPedVariationInfo* info, rage::CSyncDataBase* serializer);
		static void serialize_parachute_task(__int64 info, rage::CSyncDataBase* serializer);
		static bool received_array_update(rage::netArrayHandlerBase* array, CNetGamePlayer* sender, rage::datBitBuffer* buffer, int size, int16_t cycle);
		static bool receive_pickup(rage::netObject* netobject, void* unk, CPed* ped);
		static void* allocate_memory_reliable(rage::netConnection* cxn, int required_memory);
		static void* render_ped(__int64 renderer, CPed* ped, __int64 a3, __int64 a4);
		static void render_entity(__int64 renderer, rage::fwEntity* entity, int unk, bool a4);
		static __int64 render_big_ped(__int64 renderer, CPed* ped, __int64 a3, __int64 a4);
		static void received_clone_remove(CNetworkObjectMgr* mgr, CNetGamePlayer* src, CNetGamePlayer* dst, int16_t object_id, uint32_t ownership_token);
		static void searchlight_crash(void* a1, CPed* ped);
		static void error_packet_memmove(void* dst, void* src, int size);
		static void* create_pool_item(GenericPool* pool);

		// toxic

		static unsigned int broadcast_net_array(rage::netArrayHandlerBase* _this, CNetGamePlayer* target, rage::datBitBuffer* bit_buffer, uint16_t counter, uint32_t* elem_start, bool silent);

		// misc

		static int nt_query_virtual_memory(void* _this, HANDLE handle, PVOID base_addr, int info_class, MEMORY_BASIC_INFORMATION* info, int size, size_t* return_len);
		static int queue_dependency(void* a1, int a2, int64_t dependency);
		static bool http_start_request(void* request, const char* uri);
		static bool send_chat_message(void* team_mgr, rage::rlGamerInfo* local_gamer_info, char* message, bool is_team);
		static bool serialize_join_request_message(RemoteGamerInfoMsg* info, void* data, int size, int* bits_serialized);
		static bool serialize_join_request_message_2(CMsgJoinRequest* msg, void* buf, int size, int* bits_serialized);
		static const char* get_label_text(void* unk, const char* label);
		static bool add_gamer_to_session(rage::netConnectionManager* mgr, std::uint32_t msg_id, int* req_id, RemoteGamerInfoMsg* info, int flags, void* a6);
		static bool aimbot_cam_gameplay_director_update(uintptr_t this_);

		// spoofing

		static void write_node_data(void* data_node, rage::netObject* net_object, rage::datBitBuffer* buffer, void* log, bool update);

		// matchmaking
		static bool advertise_session(int profile_index, int num_slots, int available_slots, MatchmakingAttributes* data, std::uint64_t session_id, rage::rlSessionInfo* info, MatchmakingId* out_id, rage::rlTaskStatus* status);
		static bool update_session_advertisement(int profile_index, MatchmakingId* id, int num_slots, int available_slots, rage::rlSessionInfo* info, MatchmakingAttributes* data, rage::rlTaskStatus* status);
		static bool unadvertise_session(int profile_index, MatchmakingId* id, rage::rlTaskStatus* status);
		static void send_session_detail_msg(rage::netConnectionManager* mgr, rage::netConnection::InFrame* request_frame, rage::rlSessionDetailMsg* msg);
	};

	class minhook_keepalive
	{
	public:
		minhook_keepalive()
		{
			MH_Initialize();
		}
		~minhook_keepalive()
		{
			MH_Uninitialize();
		}
	};

	class hooking
	{
		friend hooks;

	public:
		explicit hooking();
		~hooking();

		void enable();
		void disable();

		class detour_hook_helper
		{
			friend hooking;

			using ret_ptr_fn = std::function<void*()>;

			ret_ptr_fn m_on_hooking_available = nullptr;

			detour_hook* m_detour_hook;

			void enable_hook_if_hooking_is_already_running();

			template<auto detour_function>
			struct hook_to_detour_hook_helper
			{
				static inline detour_hook m_detour_hook;
			};

		public:
			template<auto detour_function>
			static void add(const std::string& name, void* target)
			{
				hook_to_detour_hook_helper<detour_function>::m_detour_hook.set_instance(name, target, detour_function);

				detour_hook_helper d{};
				d.m_detour_hook = &hook_to_detour_hook_helper<detour_function>::m_detour_hook;

				d.enable_hook_if_hooking_is_already_running();

				m_detour_hook_helpers.push_back(d);
			}

			template<auto detour_function>
			static void* add_lazy(const std::string& name, detour_hook_helper::ret_ptr_fn on_hooking_available)
			{
				hook_to_detour_hook_helper<detour_function>::m_detour_hook.set_instance(name, detour_function);

				detour_hook_helper d{};
				d.m_detour_hook          = &hook_to_detour_hook_helper<detour_function>::m_detour_hook;
				d.m_on_hooking_available = on_hooking_available;

				d.enable_hook_if_hooking_is_already_running();

				m_detour_hook_helpers.push_back(d);

				return nullptr;
			}

			~detour_hook_helper();
		};

		template<auto detour_function>
		static auto get_original()
		{
			return detour_hook_helper::hook_to_detour_hook_helper<detour_function>::m_detour_hook.get_original<decltype(detour_function)>();
		}

	private:
		bool m_enabled{};
		minhook_keepalive m_minhook_keepalive;

		vmt_hook m_swapchain_hook;
		vtable_hook m_sync_data_reader_hook;

		call_hook m_error_packet_memmove_hook;

		WNDPROC m_og_wndproc = nullptr;

		static inline std::vector<detour_hook_helper> m_detour_hook_helpers;
	};

	inline hooking* g_hooking{};
}
