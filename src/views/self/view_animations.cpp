#include "core/vars.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "services/context_menu/context_menu_service.hpp"
#include "services/ped_animations/ped_animations_service.hpp"
#include "util/animations.hpp"
#include "views/view.hpp"

namespace big
{
	static std::vector<big::ped_animation> ped_animations;
	static std::string category = "";

	static void refresh_ped_animations()
	{
		if (category.length())
			ped_animations = g_ped_animation_service.all_saved_animations[category];
	}

	void view::animations()
	{
		static std::string search;
		static bool delete_modal;
		static float relative_pos[3], rotation[3];

		if (delete_modal)
			ImGui::OpenPopup("##deletepedanimation");

		if (ImGui::BeginPopupModal("##deletepedanimation", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove))
		{
			ImGui::Text("Are you sure you want to delete %s", g_ped_animation_service.current_animation.name.c_str());

			ImGui::Spacing();

			if (ImGui::Button("Yes"))
			{
				g_ped_animation_service.delete_saved_animation(category, g_ped_animation_service.current_animation);
				ped_animations = g_ped_animation_service.all_saved_animations[category];

				delete_modal = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("No"))
			{
				delete_modal = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::BeginGroup();
		{
			ImGui::PushItemWidth(250);
			{
				components::input_text_with_hint("##dict", "Dict", g_ped_animation_service.current_animation.dict);
				components::options_modal(
				    "All animations",
				    [] {
					    animations::list_all_anims(&g_ped_animation_service.current_animation.dict,
					        &g_ped_animation_service.current_animation.anim);
				    },
				    true,
				    "List all anims");

				components::input_text_with_hint("##anim", "Anim", g_ped_animation_service.current_animation.anim);
				ImGui::SameLine();
				components::button("Play", [] {
					g_ped_animation_service.play_saved_ped_animation(g_ped_animation_service.current_animation, self::ped, relative_pos, rotation);
				});
				ImGui::SameLine();
				components::button("Stop", [] {
					TASK::CLEAR_PED_TASKS(self::ped);
				});

				ImGui::Spacing();

				ImGui::InputFloat3("Relative Position", relative_pos);
				ImGui::SameLine();
				components::button("Reset##resetrelpos", [] {
					relative_pos[0] = relative_pos[1] = relative_pos[2] = 0;
				});

				ImGui::InputFloat3("Rotation##pos", rotation);
				ImGui::SameLine();
				components::button("Reset##resetrot", [] {
					rotation[0] = rotation[1] = rotation[2] = 0;
				});

				ImGui::Spacing();

				components::button("Get Ped Anim", [] {
					if (g_context_menu_service->m_handle)
						ped::get_anim(g_context_menu_service->m_handle);
				});
				ImGui::SameLine();
				components::button("Get Self Anim", [] {
					ped::get_anim(self::ped);
				});
			}
			ImGui::PopItemWidth();

			ImGui::Spacing();

			if (ImGui::TreeNode("Advanced Options"))
			{
				ImGui::BeginGroup(); //Regular flags
				{
					ImGui::CheckboxFlags("Looped",
					    reinterpret_cast<unsigned int*>(&g_ped_animation_service.current_animation.flags),
					    static_cast<unsigned int>(animations::anim_flags::LOOPING));
					ImGui::CheckboxFlags("Hold Last Frame",
					    reinterpret_cast<unsigned int*>(&g_ped_animation_service.current_animation.flags),
					    static_cast<unsigned int>(animations::anim_flags::HOLD_LAST_FRAME));
					ImGui::CheckboxFlags("Uninterruptable",
					    reinterpret_cast<unsigned int*>(&g_ped_animation_service.current_animation.flags),
					    static_cast<unsigned int>(animations::anim_flags::NOT_INTERRUPTABLE));
					ImGui::CheckboxFlags("Only Upperbody",
					    reinterpret_cast<unsigned int*>(&g_ped_animation_service.current_animation.flags),
					    static_cast<unsigned int>(animations::anim_flags::UPPERBODY));
					ImGui::CheckboxFlags("Secondary slot",
					    reinterpret_cast<unsigned int*>(&g_ped_animation_service.current_animation.flags),
					    static_cast<unsigned int>(animations::anim_flags::SECONDARY));
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip("Allow primary animations to run simultaniously, such as walking");
					ImGui::CheckboxFlags("Realize Animation Orientation",
					    reinterpret_cast<unsigned int*>(&g_ped_animation_service.current_animation.flags),
					    static_cast<unsigned int>(animations::anim_flags::REORIENT_WHEN_FINISHED));
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip("Use the final orientation achieved in the animation");
					ImGui::CheckboxFlags("Hide Weapon",
					    reinterpret_cast<unsigned int*>(&g_ped_animation_service.current_animation.flags),
					    static_cast<unsigned int>(animations::anim_flags::HIDE_WEAPON));
				}
				ImGui::EndGroup();
				ImGui::SameLine();
				ImGui::BeginGroup();
				{
					//Sync flags
					ImGui::CheckboxFlags("Sync In",
					    reinterpret_cast<unsigned int*>(&g_ped_animation_service.current_animation.flags),
					    static_cast<unsigned int>(animations::anim_flags::TAG_SYNC_IN));
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip("Seamless transition into the animation, for example from walking");
					ImGui::CheckboxFlags("Sync Out",
					    reinterpret_cast<unsigned int*>(&g_ped_animation_service.current_animation.flags),
					    static_cast<unsigned int>(animations::anim_flags::TAG_SYNC_OUT));
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip("Seamless transition out of the animation, for example to continue walking");
					ImGui::CheckboxFlags("Sync Continuous",
					    reinterpret_cast<unsigned int*>(&g_ped_animation_service.current_animation.flags),
					    static_cast<unsigned int>(animations::anim_flags::TAG_SYNC_CONTINUOUS));
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip("Seamless transition during the animation, especially usefull for upperbody animations");
					ImGui::CheckboxFlags("Force Start",
					    reinterpret_cast<unsigned int*>(&g_ped_animation_service.current_animation.flags),
					    static_cast<unsigned int>(animations::anim_flags::FORCE_START));
					ImGui::CheckboxFlags("Disable Colission",
					    reinterpret_cast<unsigned int*>(&g_ped_animation_service.current_animation.flags),
					    static_cast<unsigned int>(animations::anim_flags::TURN_OFF_COLLISION));
					ImGui::CheckboxFlags("Override Physics",
					    reinterpret_cast<unsigned int*>(&g_ped_animation_service.current_animation.flags),
					    static_cast<unsigned int>(animations::anim_flags::OVERRIDE_PHYSICS));
					ImGui::CheckboxFlags("Ignore Gravity",
					    reinterpret_cast<unsigned int*>(&g_ped_animation_service.current_animation.flags),
					    static_cast<unsigned int>(animations::anim_flags::IGNORE_GRAVITY));
				}
				ImGui::EndGroup();

				ImGui::TreePop();
			}
		}
		ImGui::EndGroup();
		components::hor_space();
		ImGui::BeginGroup();
		{
			ImGui::PushItemWidth(250);
			{
				components::input_text_with_hint("Category", "Category", category);

				components::input_text_with_hint("Name", "Name", g_ped_animation_service.current_animation.name);
				ImGui::SameLine();
				components::button("Save", [] {
					if (!STREAMING::DOES_ANIM_DICT_EXIST(g_ped_animation_service.current_animation.dict.data()))
						g_notification_service.push_warning("Animations",
						    std::format("Dict with the name {} does not exist",
						        g_ped_animation_service.current_animation.dict));
					else if (g_ped_animation_service.get_animation_by_name(g_ped_animation_service.current_animation.name))
						g_notification_service.push_warning("Animations",
						    std::format("Animation with the name {} already exists",
						        g_ped_animation_service.current_animation.name));
					else if (category.empty())
						g_notification_service.push_warning("Animations", "Category can't be empty");
					else if (g_ped_animation_service.current_animation.anim.empty())
						g_notification_service.push_warning("Animations", "Animation name can't be empty");
					else
					{
						g_ped_animation_service.save_new_animation(category, g_ped_animation_service.current_animation);
						refresh_ped_animations();
					}
				});

				components::button("Refresh", [] {
					g_ped_animation_service.fetch_saved_animations();
					refresh_ped_animations();
				});
				ImGui::SameLine();
				components::button("Delete Selected", [] {
					if (!g_ped_animation_service.current_animation.name.empty() && !category.empty())
						delete_modal = true;
				});

				if (components::input_text_with_hint("##search", "Search", search))
				{
					category = "";
					if (search.length() > 0)
						ped_animations = g_ped_animation_service.saved_animations_filtered_list(search);
					else
						ped_animations.clear();
				}
			}
			ImGui::PopItemWidth();
			ImGui::BeginGroup();
			{
				components::small_text("Categories");
				if (ImGui::BeginListBox("##categories", {200, static_cast<float>(*g_pointers->m_gta.m_resolution_y * 0.4)}))
				{
					if (!search.length())
						for (auto& p : g_ped_animation_service.all_saved_animations)
							if (ImGui::Selectable(p.first.c_str(), p.first == category))
							{
								category = p.first;
								refresh_ped_animations();
							}

					ImGui::EndListBox();
				}
			}
			ImGui::EndGroup();
			ImGui::SameLine();
			ImGui::BeginGroup();
			{
				components::small_text("Animations");
				if (ImGui::BeginListBox("##animations", {200, static_cast<float>(*g_pointers->m_gta.m_resolution_y * 0.4)}))
				{
					for (const auto& p : ped_animations)
					{
						if (ImGui::Selectable(p.name.data(), p.name == g_ped_animation_service.current_animation.name))
							g_ped_animation_service.current_animation = p;

						if (ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							ImGui::Text(p.name.data());
							ImGui::Text("Dict: %s\nAnim: %s", p.dict.c_str(), p.anim.c_str());
							ImGui::EndTooltip();
						}
					}

					ImGui::EndListBox();
				}
			}
			ImGui::EndGroup();
		}
		ImGui::EndGroup();
	}
}
