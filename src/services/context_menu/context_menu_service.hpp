#pragma once
#include "imgui.h"
#include "natives.hpp"
#include "util/ped.hpp"

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

		static void disable_control_action_loop();
		static void context_menu();

		Entity m_handle;
		rage::fwEntity* m_pointer{};
		model_bounding_box_screen_space m_model_bounding_box_screen_space;

		s_context_menu ped_menu{ContextEntityType::PED,
		    0,
		    {},
		    {
		        {"KILL",
		            [this] {
			            ped::kill_ped(m_handle);
		            }},
		    }};

		std::unordered_map<ContextEntityType, s_context_menu> options = {{ContextEntityType::PED, ped_menu}};
	};

	inline context_menu_service* g_context_menu_service{};
}
