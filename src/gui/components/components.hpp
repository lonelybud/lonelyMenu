#pragma once
#include "backend/bool_command.hpp"
#include "backend/command.hpp"
#include "backend/player_command.hpp"

#include <imgui.h>

namespace big
{
	struct navigation_struct;
	enum class tabs;

	class components
	{
		static void custom_text(const std::string_view, ImFont*);

	public:
		static bool nav_button(const std::string_view);
		static void small_text(const std::string_view);
		static void sub_title(const std::string_view);
		static void title(const std::string_view);
		static void nav_item(std::pair<tabs, navigation_struct>&, int);

		static bool input_text_with_hint(const std::string_view label, const std::string_view hint, char* buf, size_t buf_size, int flag = ImGuiInputTextFlags_None, std::function<void()> cb = nullptr);
		static bool input_text_with_hint(const std::string_view label, const std::string_view hint, std::string& buf, int flag = ImGuiInputTextFlags_None, std::function<void()> cb = nullptr, ImGuiInputTextCallback callback = nullptr);

		static bool input_text(const std::string_view label, char* buf, size_t buf_size, ImGuiInputTextFlags_ flag = ImGuiInputTextFlags_None, std::function<void()> cb = nullptr);
		static bool input_text(const std::string_view label, std::string& buf, ImGuiInputTextFlags_ flag = ImGuiInputTextFlags_None, std::function<void()> cb = nullptr);

		static bool selectable(const std::string_view, bool);
		static bool selectable(const std::string_view, bool, ImGuiSelectableFlags);
		static void selectable(const std::string_view, bool, std::function<void()>);
		static void selectable(const std::string_view, bool, ImGuiSelectableFlags, std::function<void()>);

		static inline void ver_space()
		{
			ImGui::Dummy(ImVec2(0.0f, ImGui::GetTextLineHeight()));
		}
		static inline void hor_space()
		{
			ImGui::SameLine(0, 2.0f * ImGui::GetTextLineHeight());
		}

		static bool script_patch_checkbox(const std::string_view text, bool* option);

		static void options_modal(const std::string_view element_name, std::function<void()> render_elements, bool sameline = true, std::string custom_button_name = "Options");

		template<template_str cmd_str, ImVec2 size = ImVec2(0, 0), ImVec4 color = ImVec4(0.24f, 0.23f, 0.29f, 1.00f)>
		static void command_button(const std::vector<uint64_t> args = {}, std::optional<const std::string_view> label_override = std::nullopt)
		{
			static command* command = command::get(rage::joaat(cmd_str.value));
			if (command == nullptr)
				return ImGui::Text("INVALID COMMAND");

			if (ImGui::Button(label_override.value_or(command->get_label()).data()))
				command->call();

			if (ImGui::IsItemHovered())
				if (auto desc = command->get_description(); desc.length())
					ImGui::SetTooltip(desc.c_str());
		}

		template<template_str cmd_str, ImVec2 size = ImVec2(0, 0), ImVec4 color = ImVec4(0.24f, 0.23f, 0.29f, 1.00f)>
		static void player_command_button(player_ptr player, const std::vector<uint64_t> args = {}, std::optional<const std::string_view> label_override = std::nullopt)
		{
			static player_command* command = static_cast<player_command*>(command::get(rage::joaat(cmd_str.value)));
			if (command == nullptr)
				return ImGui::Text("INVALID COMMAND");

			if (ImGui::Button(label_override.value_or(command->get_label()).data()))
				command->call(player);
			if (ImGui::IsItemHovered())
				if (auto desc = command->get_description(); desc.length())
					ImGui::SetTooltip(desc.c_str());
		}

		template<template_str cmd_str>
		static bool command_checkbox(std::optional<const std::string_view> label_override = std::nullopt)
		{
			static bool_command* command = static_cast<bool_command*>(command::get(rage::joaat(cmd_str.value)));
			if (command == nullptr)
			{
				ImGui::Text("INVALID COMMAND");
				return false;
			}

			bool updated;
			if (updated = ImGui::Checkbox(label_override.value_or(command->get_label()).data(), &command->is_enabled()))
				command->refresh();
			if (ImGui::IsItemHovered())
				if (auto desc = command->get_description(); desc.length())
					ImGui::SetTooltip(desc.c_str());

			return updated;
		}

		template<ImVec2 size = ImVec2(0, 0), ImVec4 color = ImVec4(0.24f, 0.23f, 0.29f, 1.00f)>
		static bool button(const std::string_view text)
		{
			bool status = false;

			if constexpr (color.x != 0.24f || color.y != 0.23f || color.z != 0.29f || color.w != 1.0f)
				ImGui::PushStyleColor(ImGuiCol_Button, color);

			status = ImGui::Button(text.data(), size);

			if constexpr (color.x != 0.24f || color.y != 0.23f || color.z != 0.29f || color.w != 1.0f)
				ImGui::PopStyleColor(1);
			return status;
		}

		template<ImVec2 size = ImVec2(0, 0), ImVec4 color = ImVec4(0.24f, 0.23f, 0.29f, 1.00f)>
		static void button(const std::string_view text, std::function<void()> cb)
		{
			if (button<size, color>(text))
			{
				g_fiber_pool->queue_job(cb);
			}
		}

		template<typename PredicateFn, typename ComponentsFn>
		static void disable_unless(PredicateFn predicate_fn, ComponentsFn components_fn)
		{
			auto const result = predicate_fn();
			if (!result)
				ImGui::BeginDisabled(true);
			components_fn();
			if (!result)
				ImGui::EndDisabled();
		}
	};
}
