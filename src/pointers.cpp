#include "pointers.hpp"

#include "gta_pointers_layout_info.hpp"
#include "memory/all.hpp"

namespace big
{
	constexpr auto pointers::get_gta_batch()
	{
		// clang-format off

        constexpr auto batch_and_hash = memory::make_batch<
        // Screen Resolution
        {
            "SR",
            "66 0F 6E 0D ? ? ? ? 0F B7 3D",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_resolution_x = ptr.sub(4).rip().as<int*>();
                g_pointers->m_gta.m_resolution_y = ptr.add(4).rip().as<int*>();
            }
        },
        // Region Code
        {
            "RC",
            "48 83 EC 28 83 3D ? ? ? ? ? 75 10",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_region_code = ptr.add(16).rip().add(1).as<uint32_t*>();
                g_pointers->m_gta.m_region_code = ptr.add(16).rip().as<uint32_t*>();
            }
        },
        // Is Session Started
        {
            "ISA",
            "40 38 35 ? ? ? ? 75 0E 4C 8B C3 49 8B D7 49 8B CE",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_is_session_started = ptr.add(3).rip().as<bool*>();
            }
        },
        // Ped Factory
        {
            "PF",
            "48 8B 05 ? ? ? ? 48 8B 48 08 48 85 C9 74 52 8B 81",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_ped_factory = ptr.add(3).rip().as<CPedFactory**>();
            }
        },
        // Network Player Manager
        {
            "NPM",
            "48 8B 0D ? ? ? ? 8A D3 48 8B 01 FF 50 ? 4C 8B 07 48 8B CF",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_network_player_mgr = ptr.add(3).rip().as<CNetworkPlayerMgr**>();
            }
        },
        // Init Native Tables & Native Handlers
        {
            "NH",
            "48 8D 0D ? ? ? ? 48 8B 14 FA E8 ? ? ? ? 48 85 C0 75 0A",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_init_native_tables        = ptr.sub(37).as<PVOID>();
                g_pointers->m_gta.m_native_registration_table = ptr.add(3).rip().as<rage::scrNativeRegistrationTable*>();
                g_pointers->m_gta.m_get_native_handler        = ptr.add(12).rip().as<functions::get_native_handler>();
            }
        },
        // Fix Vectors
        {
            "FV",
            "83 79 18 00 48 8B D1 74 4A FF 4A 18 48 63 4A 18 48 8D 41 04 48 8B 4C CA",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_fix_vectors = ptr.as<functions::fix_vectors>();
            }
        },
        // Script Threads
        {
            "ST",
            "45 33 F6 8B E9 85 C9 B8",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_script_threads     = ptr.sub(4).rip().sub(8).as<decltype(gta_pointers::m_script_threads)>();
                g_pointers->m_gta.m_run_script_threads = ptr.sub(0x1F).as<functions::run_script_threads>();
            }
        },
        // Script Programs
        {
            "SP",
            "48 8B 1D ? ? ? ? 41 83 F8 FF",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_script_program_table = ptr.add(3).rip().as<decltype(gta_pointers::m_script_program_table)>();
            }
        },
        // Script Global
        {
            "SG",
            "48 8D 15 ? ? ? ? 4C 8B C0 E8 ? ? ? ? 48 85 FF 48 89 1D",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_script_globals = ptr.add(3).rip().as<std::int64_t**>();
            }
        },
        // Swapchain
        {
            "S",
            "48 8B 0D ? ? ? ? 48 8B 01 44 8D 43 01 33 D2 FF 50 40 8B C8",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_swapchain = ptr.add(3).rip().as<IDXGISwapChain**>();
            }
        },
        // Trigger Script Event
        {
            "TSE",
            "48 8B C4 48 89 58 08 44 89 48 20 55 56 57 48 83 EC 30",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_trigger_script_event = ptr.as<decltype(gta_pointers::m_trigger_script_event)>();
            }
        },
        // Received Event Hook
        {
            "REH",
            "66 41 83 F9 ? 0F 83",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_received_event = ptr.as<decltype(gta_pointers::m_received_event)>();
            }
        },
        // Send Event Acknowledge
        {
            "SEA",
            "E8 ? ? ? ? 66 83 7B 08 5B",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_send_event_ack = ptr.add(1).rip().as<decltype(gta_pointers::m_send_event_ack)>();
            }
        },
        // Read Bitbuffer WORD/DWORD
        {
            "RBWD",
            "48 89 74 24 ? 57 48 83 EC 20 48 8B D9 33 C9 41 8B F0 8A",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_read_bitbuf_dword = ptr.sub(5).as<decltype(gta_pointers::m_read_bitbuf_dword)>();
            }
        },
        // Read Bitbuffer Array
        {
            "RBA",
            "48 89 5C 24 ? 57 48 83 EC 30 41 8B F8 4C",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_read_bitbuf_array = ptr.as<decltype(gta_pointers::m_read_bitbuf_array)>();
            }
        },
        // Read Bitbuffer String
        {
            "RBS",
            "48 89 5C 24 08 48 89 6C 24 18 56 57 41 56 48 83 EC 20 48 8B F2 45",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_read_bitbuf_string = ptr.as<decltype(gta_pointers::m_read_bitbuf_string)>();
            }
        },
        // Read Bitbuffer Boolean
        {
            "RBB",
            "48 8B C4 48 89 58 08 55 56 57 48 83 EC 20 48 83 60",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_read_bitbuf_bool = ptr.as<decltype(gta_pointers::m_read_bitbuf_bool)>();
            }
        },
        // Write Bitbuffer WORD/DWORD
        {
            "WBD",
            "48 8B C4 48 89 58 08 48 89 68 10 48 89 70 18 48 89 78 20 41 56 48 83 EC 20 8B EA BF 01",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_write_bitbuf_dword = ptr.as<decltype(gta_pointers::m_write_bitbuf_dword)>();
            }
        },
        // Write Bitbuffer QWORD
        {
            "WBQ",
            "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 20 41 8B F0 48 8B EA 48 8B D9 41 83 F8 20",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_write_bitbuf_qword = ptr.as<decltype(gta_pointers::m_write_bitbuf_qword)>();
            }
        },
        // Write Bitbuffer Int64
        {
            "WBI64",
            "E8 ? ? ? ? 8A 53 39 48 8B CF",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_write_bitbuf_int64 = ptr.add(1).rip().as<decltype(gta_pointers::m_write_bitbuf_int64)>();
            }
        },
        // Write Bitbuffer Int32
        {
            "WBI32",
            "E8 ? ? ? ? 8A 53 74",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_write_bitbuf_int32 = ptr.add(1).rip().as<decltype(gta_pointers::m_write_bitbuf_int32)>();
            }
        },
        // Write Bitbuffer Boolean
        {
            "WBB",
            "E8 ? ? ? ? 8A 57 39",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_write_bitbuf_bool = ptr.add(1).rip().as<decltype(gta_pointers::m_write_bitbuf_bool)>();
            }
        },
        // Write Bitbuffer Array
        {
            "WBA",
            "E8 ? ? ? ? 01 7E 08",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_write_bitbuf_array = ptr.add(1).rip().as<decltype(gta_pointers::m_write_bitbuf_array)>();
            }
        },
        // Ptr To Handle
        {
            "PTH",
            "48 8B F9 48 83 C1 10 33 DB",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_ptr_to_handle = ptr.sub(0x15).as<decltype(gta_pointers::m_ptr_to_handle)>();
            }
        },
        // Handle To Ptr
        {
            "GSH",
            "83 F9 FF 74 31 4C 8B 0D",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_handle_to_ptr = ptr.as<decltype(gta_pointers::m_handle_to_ptr)>();
            }
        },
        // CNetworkObjectMgr
        {
            "CNOM",
            "48 8B 0D ? ? ? ? 45 33 C0 E8 ? ? ? ? 33 FF 4C 8B F0",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_network_object_mgr = ptr.add(3).rip().as<CNetworkObjectMgr**>();
            }
        },
        // Network Player Mgr Init
        {
            "NPMI",
            "41 56 48 83 EC ? 48 8B F1 B9 ? ? ? ? 49 8B F9 41 8B E8 4C 8B F2 E8",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_network_player_mgr_init = ptr.sub(0x13).as<decltype(gta_pointers::m_network_player_mgr_init)>();
            }
        },
        // Network Player Mgr Shutdown
        {
            "NPMS",
            "48 8D 9F ? ? ? ? EB ? 48 8B 13 48 85 D2 74 ? 48 8B CB E8 ? ? ? ? 48 83 7B ? ? 75 ? 48 8D 9F",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_network_player_mgr_shutdown = ptr.sub(0x1A).as<decltype(gta_pointers::m_network_player_mgr_shutdown)>();
            }
        },
        // FriendRegistry
        {
            "FR",
            "3B 0D ? ? ? ? 73 17",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_friend_registry = ptr.add(2).rip().as<FriendRegistry*>();
            }
        },
        // GET_SCREEN_COORDS_FROM_WORLD_COORDS
        {
            "GSCFWC",
            "E8 ? ? ? ? 84 C0 74 19 F3 0F 10 44 24",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_get_screen_coords_for_world_coords = ptr.add(1).rip().as<functions::get_screen_coords_for_world_coords>();
            }
        },
        // GET_GAMEPLAY_CAM_COORDS
        {
            "GGCC",
            "8B 90 ? ? ? ? 89 13",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_get_gameplay_cam_coords = ptr.sub(0xE).as<functions::get_gameplay_cam_coords>();
            }
        },
        // Receive Net Message
        {
            "RNM",
            "48 83 EC 20 4C 8B 71 50 33 ED",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_receive_net_message = ptr.sub(0x19).as<PVOID>();
            }
        },
        // Get Network Event Data
        {
            "GNED",
            "53 43 52 49 50 54 5F 4E 45 54 57 4F 52 4B",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_get_network_event_data = *ptr.sub(0x38).as<PVOID*>();
            }
        },
        // Assign Physical Index
        {
            "API",
            "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 54 41 55 41 56 41 57 48 83 EC 20 41 8A E8",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_assign_physical_index = ptr.as<PVOID>();
            }
        },
        // Received Clone Create
        {
            "RCC",
            "48 8B C4 66 44 89 48",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_received_clone_create = ptr.as<PVOID>();
            }
        },
        // Can Apply Data
        {
            "CAD",
            "E8 ? ? ? ? 84 C0 0F 84 AF 01 00 00 48 8B 03",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_can_apply_data = ptr.add(1).rip().as<PVOID>();
            }
        },
        // Received clone sync & Get sync tree for type & Get net object for player & Get sync type info & Get net object
        {
            "RCS/GSTFT/GNOFP/GNO/GSTI",
            "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 54 41 55 41 56 41 57 48 83 EC 40 4C 8B EA",
            [](memory::handle ptr) 
            {
                g_pointers->m_gta.m_received_clone_sync = ptr.as<decltype(gta_pointers::m_received_clone_sync)>();
                g_pointers->m_gta.m_get_sync_tree_for_type = ptr.add(0x2F).add(1).rip().as<decltype(gta_pointers::m_get_sync_tree_for_type)>(); // 0F B7 CA 83 F9 07 .as()
                g_pointers->m_gta.m_get_net_object = ptr.add(0x109).add(1).rip().as<decltype(gta_pointers::m_get_net_object)>(); // E8 ? ? ? ? 0F B7 53 7C .add(1).rip().as()
            }
        },
        // Model Hash Table
        {
            "MHT",
            "4C 03 05 ? ? ? ? EB 03",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_model_table = ptr.add(3).rip().as<HashTable<CBaseModelInfo*>*>();
            }
        },
        // Get Label Text
        {
            "GLT",
            "48 8D 0D ? ? ? ? E8 ? ? ? ? 45 33 C9 41 B0 ? B2",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_get_label_text = ptr.add(8).rip().as<PVOID>();
            }
        },
        // Network
        {
            "N",
            "48 8B 0D ? ? ? ? 48 8B D7 E8 ? ? ? ? 84 C0 75 17 48 8B 0D ? ? ? ? 48 8B D7",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_network = ptr.add(3).rip().as<Network**>();
            }
        },
        // Reset Network Complaints
        {
            "RENC",
            "E8 ? ? ? ? 8B 8B ? ? ? ? 03 CF",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_reset_network_complaints = ptr.add(1).rip().as<functions::reset_network_complaints>();
            }
        },
        // fiDevice Get Device
        {
            "FDGD",
            "41 B8 07 00 00 00 48 8B F1 E8",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_fidevice_get_device = ptr.sub(0x1F).as<functions::fidevice_get_device>();
            }
        },
        // fiPackfile ctor
        {
            "FPFC",
            "44 89 41 28 4C 89 41 38 4C 89 41 50 48 8D",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_fipackfile_ctor      = ptr.sub(0x1E).as<functions::fipackfile_ctor>();
                g_pointers->m_gta.m_fipackfile_instances = ptr.add(26).rip().as<rage::fiPackfile**>();
            }
        },
        // fiPackfile dtor
        {
            "FPFD",
            "48 89 5C 24 08 57 48 83 EC 20 48 8D 05 ? ? ? ? 33 FF 48 8B D9 48 89 01 40 88",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_fipackfile_dtor = ptr.as<functions::fipackfile_dtor>();
            }
        },
        // fiPackfile stuff
        {
            "FPU&FPCA&FPOA&FPM",
            "E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 8A 05 ? ? ? ? 48 8D 0D",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_fipackfile_unmount = ptr.add(1).rip().as<functions::fipackfile_unmount>();
                g_pointers->m_gta.m_fipackfile_close_archive = ptr.add(0xD).rip().as<functions::fipackfile_close_archive>();
                g_pointers->m_gta.m_fipackfile_open_archive = ptr.add(0x34).rip().as<functions::fipackfile_open_archive>();
                g_pointers->m_gta.m_fipackfile_mount = ptr.add(0x47).rip().as<functions::fipackfile_mount>();
            }
        },
        // Send Chat Ptr
        {
            "SCP",
            "83 7E 1C 01 48 8B 3D",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_send_chat_ptr = ptr.add(7).rip().as<int64_t**>();
            }
        },
        // Send Chat Message
        {
            "SCM",
            "48 83 EC 20 48 8B F1 48 8B CA 41 8A E9",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_send_chat_message = ptr.sub(21).as<functions::send_chat_message>();
            }
        },
        // Script VM
        {
            "VM",
            "E8 ? ? ? ? 48 85 FF 48 89 1D",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_script_vm = ptr.add(1).rip().as<functions::script_vm>();
            }
        },
        // Host Token
        {
            "HT",
            "48 8B 05 ? ? ? ? 48 83 F8 FF",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_host_token = ptr.add(3).rip().as<uint64_t*>();
            }
        },
        // Profile Gamer Info
        {
            "PGI",
            "48 8D 05 ? ? ? ? 48 8B FE",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_profile_gamer_info = ptr.add(3).rip().as<rage::rlGamerInfo*>();
            }
        },
        // Player Info Gamer Info
        {
            "PIGI",
            "E8 ? ? ? ? 48 8D 4D 20 48 8B D0 E8 ? ? ? ? 41 8A CF",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_player_info_gamer_info = ptr.add(1).rip().add(3).rip().as<rage::rlGamerInfo*>();
            }
        },
        // Communications
        {
            "C",
            "48 8B 1D ? ? ? ? 48 8D 4C 24 30",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_communications = ptr.add(3).rip().as<CCommunications**>();
            }
        },
        // Handle Join Request
        {
            "HJR",
            "48 8B C4 48 89 58 08 4C 89 48 20 4C 89 40 18 48 89 50 10 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 45 33 F6",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_handle_join_request = ptr.as<PVOID>();
            }
        },
        // Write Join Response Data
        {
            "WJRD",
            "E8 ? ? ? ? 84 C0 74 07 40 84 FF 41 0F 95 C6",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_write_join_response_data = ptr.add(1).rip().as<functions::write_join_response_data>();
            }
        },
        // Queue Packet
        {
            "QP",
            "E8 ? ? ? ? 84 C0 74 4D B3 01",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_queue_packet = ptr.add(1).rip().as<functions::queue_packet>();
            }
        },
        // Serialize Join Request Message
        {
            "SJRM",
            "E8 ? ? ? ? 84 C0 0F 84 9B 00 00 00 49 8D 8F 48 11 00 00",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_serialize_join_request_message = ptr.add(1).rip().as<PVOID>();
            }
        },
        // Serialize Join Request Message 2
        {
            "SJRM2",
            "E8 ? ? ? ? 48 8D 8D 08 01 00 00 8A D8",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_serialize_join_request_message_2 = ptr.add(1).rip().as<PVOID>();
            }
        },
        // Request Control
        {
            "RC",
            "E8 ? ? ? ? EB 3E 48 8B D3",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_request_control = ptr.add(1).rip().as<functions::request_control>();
            }
        },
        // Get Connection Peer
        {
            "GCP",
            "48 89 5C 24 08 48 89 74 24 18 89 54 24 10 57 48 83 EC 40 48",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_get_connection_peer = ptr.as<functions::get_connection_peer>();
            }
        },
        // Handle Remove Gamer Command
        {
            "HRGC",
            "48 85 D2 0F 84 0E 04",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_handle_remove_gamer_cmd = ptr.as<functions::handle_remove_gamer_cmd>();
            }
        },
        // Serialize Take Off Ped Variation Task
        {
            "STOPVT",
            "40 55 53 57 41 56 48 8B EC 48 83 EC 68",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_serialize_take_off_ped_variation_task = ptr.as<PVOID>();
            }
        },
        // Serialize Parachute Task
        {
            "SPT",
            "40 55 53 56 57 41 54 48 8B",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_serialize_parachute_task = ptr.as<PVOID>();
            }
        },
        // Invalid Decal Crash
        {
            "IDC",
            "E8 ? ? ? ? 8B 9C 24 B8 00 00 00 4C 8B AC 24 A8 00 00 00",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_invalid_decal_crash = ptr.add(1).rip().as<PVOID>();
            }
        },
        // Task Parachute Object
        {
            "TPO",
            "0F 88 ? ? ? ? 75 34",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_task_parachute_object = ptr.sub(6).as<PVOID>();
            }
        },
        // Task Ambient Clips
        {
            "TAC",
            "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 80 3D ? ? ? ? ? 41 8B D8 8B F2",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_task_ambient_clips = ptr.as<PVOID>();
            }
        },
        // Queue Dependency
        {
            "QD",
            "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 8B F2 49 8B F8",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_queue_dependency = ptr.as<PVOID>();
            }
        },
        // Prepare Metric For Sending
        {
            "PMFS",
            "48 8B C4 48 89 58 08 48 89 68 10 48 89 70 18 48 89 78 20 41 56 48 83 EC 30 49 8B E8 4C 8D 40 EC 49 8B F1 48 8B D9 40 32 FF E8",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_prepare_metric_for_sending = ptr.as<PVOID>();
            }
        },
        // HTTP Start Request
        {
            "HSR",
            "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B D9 48 81 C1 ? ? ? ? 48 8B F2 33 FF E8",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_http_start_request = ptr.as<PVOID>();
            }
        },
        // Send Packet
        {
            "SP",
            "48 8B C4 48 89 58 08 48 89 70 10 48 89 78 18 4C 89 48 20 55 41 54 41 55 41 56 41 57 48 8D A8 98",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_send_packet = ptr.as<functions::send_packet>();
            }
        },
        // Fragment Physics Crash 2
        {
            "FPC2",
            "E8 ? ? ? ? 84 C0 75 0B 41 FF CF",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_fragment_physics_crash_2 = ptr.add(1).rip().as<PVOID>();
            }
        },
        // Received Array Update
        {
            "RAU",
            "48 89 5C 24 10 55 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC 30 48 8B 05",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_received_array_update = ptr.as<PVOID>();
            }
        },
        // Receive Pickup
        {
            "RPI",
            "49 8B 80 ? ? ? ? 48 85 C0 74 0C F6 80 ? ? ? ? ? 75 03 32 C0 C3",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_receive_pickup = ptr.as<PVOID>();
            }
        },
        // Game Version + Online Version
        {
            "GVOV",
            "8B C3 33 D2 C6 44 24 20",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_game_version   = ptr.add(0x24).rip().as<const char*>();
                g_pointers->m_gta.m_online_version = ptr.add(0x24).rip().add(0x20).as<const char*>();
            }
        },
        // Ped Pool
        {
            "PEP",
            "4C 8B 35 ? ? ? ? B8 ? ? ? ? 0F 57 F6 89 05 ? ? ? ? 49 63 76 10 4C 8B FE 85 F6 0F 84 ? ? ? ? 49 8B 46 08 49 FF CF FF CE 42 0F B6 0C 38",
            [](memory::handle ptr)
		    {
			    g_pointers->m_gta.m_ped_pool = ptr.add(3).rip().as<GenericPool**>();
		    }
        },
        // Prop Pool
        {
            "PRP",
            "48 8B 05 ? ? ? ? 0F B7 50 10 48 8B 05",
            [](memory::handle ptr)
		    {
			    g_pointers->m_gta.m_prop_pool = ptr.add(3).rip().as<GenericPool**>();
		    }
        },
        // Vehicle Pool
        {
            "VEP",
            "4C 8B 25 ? ? ? ? 8B 29 33 F6 49 8B 04 24 33 DB 4C 8D 71 08 44 8B 78 08 45 85 FF 0F 8E ? ? ? ? 4D 8B 0C 24 41 3B 59 08 7D 29 49 8B 51 30 44 8B C3 8B CB 49 C1 E8 05 83 E1 1F 44 8B D3 42 8B 04 82",
            [](memory::handle ptr)
		    {
			    g_pointers->m_gta.m_vehicle_pool = ptr.add(3).rip().as<VehiclePool***>();
		    }
        },
        // Get Host Array Handler By Index
        {
            "GHAHBI",
            "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 20 8A 81 8F",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_get_host_array_handler_by_index = ptr.as<functions::get_host_array_handler_by_index>();
            }
        },
        // Allocate Memory Reliable & Connection Manager Try Free Memory
        {
            "AMR&CMTFM",
            "48 8B C4 48 89 58 08 48 89 68 10 48 89 70 18 48 89 78 20 41 56 48 83 EC 20 48 8B D9 48 8B 49 18",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_allocate_memory_reliable = ptr.as<PVOID>();
                g_pointers->m_gta.m_connection_manager_try_free_memory = ptr.add(0x52).rip().as<functions::connection_manager_try_free_memory>();
            }
        },
        // Remove Message From Queue & Remove Message From Unacked Reliables
        {
            "RMFQ&RMFUR",
            "E8 ? ? ? ? 0F B7 43 4C 48 8D 55 20",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_remove_message_from_queue = ptr.add(1).rip().as<functions::remove_message_from_queue>();
                g_pointers->m_gta.m_remove_message_from_unacked_reliables = ptr.add(0x19).rip().as<functions::remove_message_from_unacked_reliables>();
            }
        },
        // Draw Handler Manager
        {
            "DHM",
            "48 89 05 ? ? ? ? EB 07 48 89 1D ? ? ? ? 48 8B CB",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_draw_handler_mgr = ptr.add(3).rip().as<PVOID*>();
            }
        },
        // Render Ped
        {
            "RP",
            "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 54 41 55 41 56 41 57 48 81 EC 80 00 00 00 48 8B FA",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_render_ped = ptr.as<PVOID*>();
            }
        },
        // Render Entity
        {
            "RE",
            "48 8B C4 48 89 58 08 48 89 68 10 48 89 70 18 57 41 54 41 55 41 56 41 57 48 83 EC 70 0F BA",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_render_entity = ptr.as<PVOID*>();
            }
        },
        // Render Big Ped
        {
            "RE",
            "48 89 5C 24 08 4C 89 44 24 18 55 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 81 EC 80 00 00 00 48",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_render_big_ped = ptr.as<PVOID*>();
            }
        },
        // Remove Reference
        {
            "RR",
            "48 89 5C 24 08 57 48 83 EC 20 80 3D ? ? ? ? ? 48 8B FA 48 8B D9 74 13",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_remove_reference = ptr.as<functions::remove_reference>();
            }
        },
        // Sync Data Reader Vtable
        {
            "RBS",
            "48 8D 05 ? ? ? ? 48 8D 54 24 20 48 89 44 24 20 48 8D 44 24 40",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_sync_data_reader_vtable = ptr.add(3).rip().as<void**>();
            }
        },
        // Interior Proxy Pool
        {
            "IPP",
            "4C 8B 05 ? ? ? ? 4C 0F BF 0B",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_interior_proxy_pool = ptr.add(3).rip().as<GenericPool**>();
            }
        },
        // Train Config Array
        {
            "TCA",
            "48 8D 0D ? ? ? ? E8 ? ? ? ? 44 88 64 24 30 4C 8D 8C 24 60 02 00 00",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_train_config_array = ptr.add(3).rip().as<rage::atArray<CTrainConfig>*>();
            }
        },
        // Is Matchmaking Session Valid
        {
            "IMSV",
            "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 54 41 55 41 56 41 57 48 83 EC 20 45 0F",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_is_matchmaking_session_valid = ptr;
            }
        },
        // Creator Warp Cheat Triggered Patch
        {
            "CW",
            "74 44 E8 ? ? ? ? 80 65 2B F8 48 8D 0D ? ? ? ? 48 89 4D 17 48 89 7D 1F 89 7D 27 C7 45",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_creator_warp_cheat_triggered_patch = ptr;
            }
        },
        // Sound Overload Detour
        {
            "SOD",
            "66 45 3B C1 74 38",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_sound_overload_detour = ptr;
            }
        },
        // Crash Trigger
        {
            "CT",
            "48 3B F8 74 ? 8B 1D",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_crash_trigger = ptr;
            }
        },
        // Script VM Patch 1
        {
            "SVM1",
            "3b 0a 0f 83 ? ? ? ? 48 ff c7",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_script_vm_patch_1 = ptr;
            }
        },
        // Script VM Patch 2
        {
            "SVM2",
            "3b 0a 0f 83 ? ? ? ? 49 03 fa",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_script_vm_patch_2 = ptr;
            }
        },
        // Script VM Patches 3 and 4
        {
            "SVM3&4",
            "3b 11 0f 83 ? ? ? ? 48 ff c7",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_script_vm_patch_3 = ptr;
                g_pointers->m_gta.m_script_vm_patch_4 = ptr.add(0x1C);
            }
        },
        // Script VM Patches 5 and 6
        {
            "SVM5&6",
            "3b 11 0f 83 ? ? ? ? 49 03 fa",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_script_vm_patch_5 = ptr;
                g_pointers->m_gta.m_script_vm_patch_6 = ptr.add(0x26);
            }
        },
        // CTheScripts::GetHandlerCheckNetwork patch (aka Model Spawn Bypass)
        {
            "CTSHP",
            "48 8B C8 FF 52 30 84 C0 74 05 48",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_model_spawn_bypass = ptr.add(8).as<PVOID>();
            }
        },
        // Blip List
        {
            "BLPLST",
            "4C 8D 05 ? ? ? ? 0F B7 C1",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_blip_list = ptr.add(3).rip().as<CBlipList*>();
            }
        },
        // Free Event Error
        {
            "FEE",
            "48 8B 5C 24 40 48 8B 6C 24 48 48 8B 74 24 50 48 8B 7C 24 58 48 83 C4 30 41 5E C3 48 8B 0D",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_free_event_error = ptr.add(0x31).as<PVOID>();
            }
        },
        // Activate Special Ability Patch
        {
            "ASAP",
            "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 0F B7 49 30 48 8B F2 E8 ? ? ? ? 33 DB 48 8B F8 48 85 C0 74 35",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_activate_special_ability_patch = ptr.as<PVOID>();
            }
        },
        // Game Skeleton
        {
            "GS",
            "48 8D 0D ? ? ? ? BA ? ? ? ? 74 05 BA ? ? ? ? E8 ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? ? 48 8D 0D ? ? ? ? BA ? ? ? ? 84 DB 75 05 BA ? ? ? ? E8 ? ? ? ? 48 8B CD C6 05 ? ? ? ? ? E8 ? ? ? ? 84",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_game_skeleton = ptr.add(3).rip().as<rage::game_skeleton*>();
            }
        },
        // Nullsub
        {
            "NS",
            "90 C3",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_nullsub = ptr.as<void(*)()>();
            }
        },
        // Get Ped Seat
        {
            "GPS",
            "E8 ? ? ? ? 48 85 DB 74 66",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_get_ped_seat = ptr.add(1).rip().as<functions::get_ped_seat>();
            }
        },
        // RECEIVED_CLONE_REMOVE
        {
            "RCR",
            "48 8B C4 48 89 58 08 48 89 68 10 48 89 70 18 48 89 78 20 41 54 41 56 41 57 48 83 EC 50 4C 8B F2 4D 8B E0",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_received_clone_remove = ptr.as<functions::received_clone_remove>();
            }
        },
        // CWeaponInfoManager
        {
            "CWIM",
            "0F B7 15 ? ? ? ? ? 33 D2 2B D3 78 ? ? 8B 1D",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_weapon_info_manager = ptr.add(3).rip().sub(72).as<CWeaponInfoManager*>();
            }
        },
        // Searchlight Crash
        {
            "SLC",
            "0F 29 70 E8 0F 29 78 D8 48 8B F9 48 8B CA",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_searchlight_crash = ptr.sub(0x1E).as<PVOID>();
                g_pointers->m_gta.m_get_searchlight = ptr.add(0x28).rip().as<functions::get_searchlight>();
            }
        },
        // Vehicle Allocator
        {
            "VA",
            "48 8B 0D ? ? ? ? 45 33 C9 BA ? ? ? ? 41",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_vehicle_allocator = ptr.add(3).rip().as<GenericPool**>();
            }
        },
        // Write Node Data
        {
            "WND",
            "48 8B 89 A8 00 00 00 4C 8B 11 49 FF 62 10",
            [](memory::handle ptr)
            {
                g_pointers->m_gta.m_write_node_data = ptr.as<PVOID>();
            }
        }
        >(); // don't leave a trailing comma at the end

		// clang-format on

		return batch_and_hash;
	}

	void pointers::load_pointers_from_cache(const cache_file& cache_file, const uintptr_t pointer_to_cacheable_data_start, const memory::module& mem_region)
	{
		// fill pointers instance fields by reading the file data into it

		LOG(INFO) << "Loading pointers instance from cache";

		// multiple things here:
		// - iterate each cacheable field of the pointers instance
		// - add the base module address to the current offset retrieved from the cache
		// - assign that ptr to the pointers field
		uintptr_t* cache_data = reinterpret_cast<uintptr_t*>(cache_file.data());

		const size_t field_count_from_cache = cache_file.data_size() / sizeof(uintptr_t);
		LOG(INFO) << "Pointers cache: Loading " << field_count_from_cache << " fields from the cache";

		uintptr_t* field_ptr = reinterpret_cast<uintptr_t*>(pointer_to_cacheable_data_start);
		for (size_t i = 0; i < field_count_from_cache; i++)
		{
			uintptr_t offset = cache_data[i];
			uintptr_t ptr    = offset + mem_region.begin().as<uintptr_t>();

			if (mem_region.contains(memory::handle(ptr)))
			{
				*field_ptr = ptr;
			}
			else
			{
				LOG(FATAL) << "Just tried to load from cache a pointer supposedly within the gta 5 module range but isn't! Offset from start of pointers instance: " << (reinterpret_cast<uintptr_t>(field_ptr) - reinterpret_cast<uintptr_t>(this));
			}

			field_ptr++;
		}
	}

	pointers::pointers() :
	    m_gta_pointers_cache(g_file_manager.get_project_file("./cache/gta_pointers.bin")),
	    m_sc_pointers_cache(g_file_manager.get_project_file("./cache/sc_pointers.bin"))
	{
		g_pointers = this;

		const auto mem_region = memory::module("GTA5.exe");

		constexpr auto gta_batch_and_hash = pointers::get_gta_batch();
		constexpr cstxpr_str gta_batch_name{"GTA5"};
		write_to_cache_or_read_from_cache<gta_batch_name,
		    gta_batch_and_hash.m_hash,
		    gta_pointers_layout_info::offset_of_cache_begin_field,
		    gta_pointers_layout_info::offset_of_cache_end_field,
		    gta_batch_and_hash.m_batch>(m_gta_pointers_cache, mem_region);

		m_hwnd = FindWindowW(L"grcWindow", nullptr);

		if (!m_hwnd)
			throw std::runtime_error("Failed to find the game's window.");
	}

	pointers::~pointers()
	{
		g_pointers = nullptr;
	}
}
