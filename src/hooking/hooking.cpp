#include "hooking.hpp"

#include "pointers.hpp"

namespace big
{
	hooking::hooking() :
	    m_swapchain_hook(*g_pointers->m_gta.m_swapchain, hooks::swapchain_num_funcs),
	    m_sync_data_reader_hook(g_pointers->m_gta.m_sync_data_reader_vtable, 27),
	    m_error_packet_memmove_hook(g_pointers->m_gta.m_error_packet_memmove, hooks::error_packet_memmove)
	{
		m_swapchain_hook.hook(hooks::swapchain_present_index, &hooks::swapchain_present);
		m_swapchain_hook.hook(hooks::swapchain_resizebuffers_index, &hooks::swapchain_resizebuffers);

		// The only instances in that vector at this point should only be the "lazy" hooks
		// aka the ones that still don't have their m_target assigned
		for (auto& detour_hook_helper : m_detour_hook_helpers)
		{
			detour_hook_helper.m_detour_hook->set_target_and_create_hook(detour_hook_helper.m_on_hooking_available());
		}

		detour_hook_helper::add<hooks::run_script_threads>("SH", g_pointers->m_gta.m_run_script_threads);

		detour_hook_helper::add<hooks::get_label_text>("GLT", g_pointers->m_gta.m_get_label_text);

		detour_hook_helper::add<hooks::init_native_tables>("INT", g_pointers->m_gta.m_init_native_tables);
		detour_hook_helper::add<hooks::script_vm>("SVM", g_pointers->m_gta.m_script_vm);

		detour_hook_helper::add<hooks::network_player_mgr_init>("NPMI", g_pointers->m_gta.m_network_player_mgr_init);
		detour_hook_helper::add<hooks::network_player_mgr_shutdown>("NPMS", g_pointers->m_gta.m_network_player_mgr_shutdown);

		detour_hook_helper::add<hooks::received_event>("RE", g_pointers->m_gta.m_received_event);

		detour_hook_helper::add<hooks::assign_physical_index>("API", g_pointers->m_gta.m_assign_physical_index);

		detour_hook_helper::add<hooks::receive_net_message>("RNM", g_pointers->m_gta.m_receive_net_message);

		detour_hook_helper::add<hooks::received_clone_create>("RCC", g_pointers->m_gta.m_received_clone_create);
		detour_hook_helper::add<hooks::received_clone_sync>("RCS", g_pointers->m_gta.m_received_clone_sync);
		detour_hook_helper::add<hooks::can_apply_data>("CAD", g_pointers->m_gta.m_can_apply_data);

		detour_hook_helper::add<hooks::get_network_event_data>("GNED", g_pointers->m_gta.m_get_network_event_data);

		detour_hook_helper::add<hooks::invalid_decal>("IDC", g_pointers->m_gta.m_invalid_decal_crash);
		detour_hook_helper::add<hooks::task_parachute_object>("TPO", g_pointers->m_gta.m_task_parachute_object);
		detour_hook_helper::add<hooks::task_ambient_clips>("TAC", g_pointers->m_gta.m_task_ambient_clips);

		detour_hook_helper::add<hooks::handle_join_request>("HJR", g_pointers->m_gta.m_handle_join_request);

		detour_hook_helper::add<hooks::send_chat_message>("SCM", g_pointers->m_gta.m_send_chat_message);

		detour_hook_helper::add<hooks::broadcast_net_array>("BNA", g_pointers->m_gta.m_broadcast_net_array);

		detour_hook_helper::add<hooks::serialize_join_request_message>("SJRM", g_pointers->m_gta.m_serialize_join_request_message);
		detour_hook_helper::add<hooks::serialize_join_request_message_2>("SJRM2", g_pointers->m_gta.m_serialize_join_request_message_2);

		detour_hook_helper::add<hooks::serialize_take_off_ped_variation_task>("STOPVT", g_pointers->m_gta.m_serialize_take_off_ped_variation_task);
		detour_hook_helper::add<hooks::serialize_parachute_task>("SPT", g_pointers->m_gta.m_serialize_parachute_task);

		detour_hook_helper::add<hooks::queue_dependency>("QD", g_pointers->m_gta.m_queue_dependency);
		detour_hook_helper::add<hooks::prepare_metric_for_sending>("PMFS", g_pointers->m_gta.m_prepare_metric_for_sending);
		detour_hook_helper::add<hooks::http_start_request>("HSR", g_pointers->m_gta.m_http_start_request);

		detour_hook_helper::add<hooks::fragment_physics_crash_2>("FPC2", g_pointers->m_gta.m_fragment_physics_crash_2);

		detour_hook_helper::add<hooks::received_array_update>("RAU", g_pointers->m_gta.m_received_array_update);

		detour_hook_helper::add<hooks::receive_pickup>("RPI", g_pointers->m_gta.m_receive_pickup);

		detour_hook_helper::add<hooks::allocate_memory_reliable>("AMR", g_pointers->m_gta.m_allocate_memory_reliable);

		detour_hook_helper::add<hooks::render_ped>("RP", g_pointers->m_gta.m_render_ped);
		detour_hook_helper::add<hooks::render_entity>("RE", g_pointers->m_gta.m_render_entity);
		detour_hook_helper::add<hooks::render_big_ped>("RBP", g_pointers->m_gta.m_render_big_ped);

		detour_hook_helper::add<hooks::received_clone_remove>("RCR", g_pointers->m_gta.m_received_clone_remove);

		detour_hook_helper::add<hooks::aimbot_cam_gameplay_director_update>("CGDU", g_pointers->m_gta.m_cam_gameplay_director_update);

		detour_hook_helper::add<hooks::searchlight_crash>("SLC", g_pointers->m_gta.m_searchlight_crash);

		detour_hook_helper::add<hooks::advertise_session>("AS", g_pointers->m_gta.m_advertise_session);
		detour_hook_helper::add<hooks::update_session_advertisement>("USA", g_pointers->m_gta.m_update_session_advertisement);
		detour_hook_helper::add<hooks::unadvertise_session>("US", g_pointers->m_gta.m_unadvertise_session);
		detour_hook_helper::add<hooks::send_session_detail_msg>("SSDM", g_pointers->m_gta.m_send_session_detail_msg);

		detour_hook_helper::add<hooks::write_node_data>("WND", g_pointers->m_gta.m_write_node_data);
  
		detour_hook_helper::add<hooks::add_gamer_to_session>("AGTS", g_pointers->m_gta.m_add_gamer_to_session);

		detour_hook_helper::add<hooks::create_pool_item>("CPI", g_pointers->m_gta.m_create_pool_item);

		g_hooking = this;
	}

	hooking::~hooking()
	{
		if (m_enabled)
		{
			disable();
		}

		g_hooking = nullptr;
	}

	void hooking::enable()
	{
		m_swapchain_hook.enable();
		m_sync_data_reader_hook.enable();
		m_error_packet_memmove_hook.enable();
		m_og_wndproc = WNDPROC(SetWindowLongPtrW(g_pointers->m_hwnd, GWLP_WNDPROC, LONG_PTR(&hooks::wndproc)));

		for (auto& detour_hook_helper : m_detour_hook_helpers)
		{
			detour_hook_helper.m_detour_hook->enable();
		}

		MH_ApplyQueued();

		m_enabled = true;
	}

	void hooking::disable()
	{
		m_enabled = false;

		for (auto& detour_hook_helper : m_detour_hook_helpers)
		{
			detour_hook_helper.m_detour_hook->disable();
		}

		SetWindowLongPtrW(g_pointers->m_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(m_og_wndproc));
		m_error_packet_memmove_hook.disable();
		m_sync_data_reader_hook.disable();
		m_swapchain_hook.disable();

		MH_ApplyQueued();

		m_detour_hook_helpers.clear();
	}

	hooking::detour_hook_helper::~detour_hook_helper()
	{
	}

	void hooking::detour_hook_helper::enable_hook_if_hooking_is_already_running()
	{
		if (g_hooking && g_hooking->m_enabled)
		{
			if (m_on_hooking_available)
			{
				m_detour_hook->set_target_and_create_hook(m_on_hooking_available());
			}

			m_detour_hook->enable();
			MH_ApplyQueued();
		}
	}
}
