#include "core/data/ptfx_effects.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "services/notifications/notification_service.hpp"
#include "util/strings.hpp"
#include "views/view.hpp"
#include "core/vars.hpp"

using json = nlohmann::json;

namespace big
{
	namespace ptfxEffects
	{
		struct Effect
		{
			std::string name;
			std::string dict;
			std::string effect;
			float size; // default size
		};

		struct AnimDictCompactObj
		{
			std::string DictionaryName;
			std::vector<std::string> EffectNames;
		};

		NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Effect, name, dict, effect, size);
		NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AnimDictCompactObj, DictionaryName, EffectNames);

		std::filesystem::path getSavedEffectsFilePath()
		{
			return big::g_file_manager.get_project_file("savedPtfxEffects.json").get_path();
		}
		void loadEffects(std::vector<AnimDictCompactObj>& list)
		{
			try
			{
				auto filePath = big::g_file_manager.get_project_file("particleEffectsCompact.json").get_path();

				if (std::filesystem::exists(filePath))
				{
					std::ifstream f(filePath);

					if (f.is_open())
					{
						list = json::parse(f);
						f.close();
					}
				}
				else
					big::g_notification_service.push_warning("Sorry", "Please download particleEffectsCompact.json.");
			}
			catch (std::exception e)
			{
				LOG(WARNING) << e.what();
			}
		}
		void loadSavedEffects(std::map<std::string, std::vector<Effect>>& list)
		{
			auto savedFilePath = getSavedEffectsFilePath();
			list.clear();
			try
			{
				if (std::filesystem::exists(savedFilePath))
				{
					std::ifstream f(savedFilePath);
					if (f.is_open())
					{
						list = json::parse(f);
						f.close();
					}
				}
			}
			catch (std::exception e)
			{
				LOG(WARNING) << e.what();
			}
		}
		void saveEffects(std::map<std::string, std::vector<Effect>>& list)
		{
			auto savedFilePath = getSavedEffectsFilePath();

			try
			{
				json j = list;

				std::ofstream o(savedFilePath);
				if (o.is_open())
				{
					o << std::setw(4) << j << std::endl;
					o.close();
				}
			}
			catch (std::exception e)
			{
				LOG(WARNING) << e.what();
			}
		}
	}

	auto filterStrings(const std::vector<ptfxEffects::AnimDictCompactObj> vec, const std::string& search_string)
	{
		std::vector<ptfxEffects::AnimDictCompactObj> res;

		for (auto some : vec)
		{
			std::string dic_name = some.DictionaryName;
			std::transform(dic_name.begin(), dic_name.end(), dic_name.begin(), ::tolower);

			if (dic_name.find(search_string) != std::string::npos)
				res.push_back(some);
		}

		return res;
	}

	static inline void set_effect(const char* asset, const char* effect)
	{
		g_ptfx_effects.asset  = asset;
		g_ptfx_effects.effect = effect;
	}

	void view::ptfx_effects()
	{
		static std::vector<ptfxEffects::AnimDictCompactObj> animDictCompactObjs, searchedAnimDictCompactObjs;
		static std::map<std::string, std::vector<ptfxEffects::Effect>> savedEffects;
		static std::string searchDicText;
		static int selecDicIndex = -1;
		static std::string groupToDelete;
		static int customEffectToDeleteIndex = -1;
		static std::string groupName, customEffectName, selectedGroupName;

		ImGui::Checkbox("Enable PTFX", &g_ptfx_effects.show);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(400);
		ImGui::SliderFloat("PTFX Size", &g_ptfx_effects.size, 0.1f, 2.f);

		ImGui::PushItemWidth(200);
		ImGui::InputInt("PTFX Delay (ms)", &g_ptfx_effects.delay);
		ImGui::SameLine();
		ImGui::Checkbox("Center##ptfx", &g_ptfx_effects.center);

		ImGui::Spacing();
		static float custom_location[3];
		ImGui::InputFloat3("###Customlocation", custom_location);
		ImGui::SameLine();
		components::button("Set curr. coord", [&] {
			custom_location[0] = self::pos.x;
			custom_location[1] = self::pos.y;
			custom_location[2] = self::pos.z;
		});
		ImGui::SameLine();
		components::button("Trigger ptfx", [&] {
			STREAMING::REQUEST_NAMED_PTFX_ASSET(g_ptfx_effects.asset);
			GRAPHICS::USE_PARTICLE_FX_ASSET(g_ptfx_effects.asset);
			GRAPHICS::START_NETWORKED_PARTICLE_FX_NON_LOOPED_AT_COORD(g_ptfx_effects.effect,
			    custom_location[0],
			    custom_location[1],
			    custom_location[2],
			    0.f,
			    0.f,
			    0.f,
			    g_ptfx_effects.size,
			    0.f,
			    0.f,
			    0.f,
			    0);
		});

		ImGui::Spacing();

		auto& objs = searchDicText.length() > 0 ? searchedAnimDictCompactObjs : animDictCompactObjs;

		ImGui::BeginGroup();
		{
			ImGui::SetNextItemWidth(200);
			if (components::input_text_with_hint("##searchDicText", "searchDic", searchDicText))
			{
				if (searchDicText.length() > 0)
				{
					std::transform(searchDicText.begin(), searchDicText.end(), searchDicText.begin(), ::tolower);
					searchedAnimDictCompactObjs = filterStrings(animDictCompactObjs, searchDicText);
				}
				else
					searchedAnimDictCompactObjs.clear();

				selecDicIndex = -1;
			}
			ImGui::SameLine();
			components::button("Load ptfx effects", [&] {
				ptfxEffects::loadEffects(animDictCompactObjs);
			});

			ImGui::BeginGroup();
			components::small_text("DictionaryName");
			if (ImGui::BeginListBox("##dictionaries", {200, static_cast<float>(*g_pointers->m_gta.m_resolution_y * 0.4)}))
			{
				unsigned i = 0;
				for (auto& dicObj : objs)
				{
					if (ImGui::Selectable(dicObj.DictionaryName.c_str(), selecDicIndex == i))
						selecDicIndex = i;
					++i;
				}

				ImGui::EndListBox();
			}
			ImGui::EndGroup();
			ImGui::SameLine();
			ImGui::BeginGroup();
			components::small_text("EffectNames");
			if (selecDicIndex != -1 && ImGui::BeginListBox("##effectNames", {200, static_cast<float>(*g_pointers->m_gta.m_resolution_y * 0.4)}))
			{
				for (auto& effectName : objs[selecDicIndex].EffectNames)
					if (ImGui::Selectable(effectName.c_str(), effectName == g_ptfx_effects.effect))
						set_effect(objs[selecDicIndex].DictionaryName.c_str(), effectName.c_str());

				ImGui::EndListBox();
			}
			ImGui::EndGroup();
		}
		ImGui::EndGroup();
		ImGui::SameLine();
		ImGui::BeginGroup();
		{
			ImGui::SeparatorText("Save an effect");

			ImGui::PushItemWidth(200);
			components::input_text_with_hint("##groupName", "groupName", groupName);
			components::input_text_with_hint("##customEffectName", "customEffectName", customEffectName);
			ImGui::PopItemWidth();
			components::button("save", [&] {
				std::string _groupName = trimString(groupName);
				std::string effectName = trimString(customEffectName);
				std::string currDict   = g_ptfx_effects.asset;
				std::string currEffect = g_ptfx_effects.effect;

				if (_groupName.size() > 0 && effectName.size() > 0 && currDict.size() > 0 && currEffect.size() > 0)
				{
					ptfxEffects::Effect effect = {effectName, currDict, currEffect, g_ptfx_effects.size};

					if (savedEffects.size() == 0)
						ptfxEffects::loadSavedEffects(savedEffects);

					if (savedEffects.find(_groupName) != savedEffects.end())
					{
						auto& effects = savedEffects[_groupName];

						auto it = std::find_if(effects.begin(), effects.end(), [&effectName](const ptfxEffects::Effect& effect) {
							return effect.name == effectName;
						});

						// if effect already present
						if (it != effects.end())
							*it = effect;
						else
							savedEffects[_groupName].push_back(effect);
					}
					else
						savedEffects[_groupName] = std::vector({effect});

					ptfxEffects::saveEffects(savedEffects);
				}
			});
			ImGui::SameLine();
			components::button("Load saved effects", [&] {
				ptfxEffects::loadSavedEffects(savedEffects);
			});

			components::small_text("Shift click to delete a groupName or effect inside it");

			ImGui::BeginGroup();
			components::small_text("groupNames");
			if (ImGui::BeginListBox("##groupNames", {200, static_cast<float>(*g_pointers->m_gta.m_resolution_y * 0.4)}))
			{
				for (auto& obj : savedEffects)
					if (ImGui::Selectable(obj.first.c_str(), obj.first == selectedGroupName))
					{
						if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
						{
							groupToDelete             = obj.first;
							customEffectToDeleteIndex = -1;
						}
						else
							selectedGroupName = groupName = obj.first;
					}
				ImGui::EndListBox();
			}
			ImGui::EndGroup();
			ImGui::SameLine();
			ImGui::BeginGroup();
			components::small_text("customEffectNames");
			if (selectedGroupName.length()
			    && ImGui::BeginListBox("##customEffectNames", {200, static_cast<float>(*g_pointers->m_gta.m_resolution_y * 0.4)}))
			{
				for (int i = 0; i < savedEffects[selectedGroupName].size(); ++i)
				{
					auto& effect = savedEffects[selectedGroupName][i];

					if (ImGui::Selectable(effect.name.c_str(), effect.effect.c_str() == g_ptfx_effects.effect))
					{
						if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
						{
							groupToDelete             = selectedGroupName;
							customEffectToDeleteIndex = i;
						}
						else
						{
							customEffectName = effect.name;
							set_effect(effect.dict.c_str(), effect.effect.c_str());
							g_ptfx_effects.size = effect.size;
						}
					}
				}
				ImGui::EndListBox();
			}
			ImGui::EndGroup();
		}
		ImGui::EndGroup();

		if (!std::string(groupToDelete).empty() || customEffectToDeleteIndex != -1)
			ImGui::OpenPopup("##deleteCustomEffectOrGroup");
		if (ImGui::BeginPopupModal("##deleteCustomEffectOrGroup", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove))
		{
			ImGui::Text(std::format("Are you sure you want to delete {}", (customEffectToDeleteIndex != -1 ? "effect" : "group"))
			                .c_str());
			ImGui::Spacing();
			if (ImGui::Button("Yes"))
			{
				if (customEffectToDeleteIndex != -1)
					savedEffects[groupToDelete].erase(savedEffects[groupToDelete].begin() + customEffectToDeleteIndex);
				else
				{
					savedEffects.erase(groupToDelete);
					if (selectedGroupName == groupToDelete || savedEffects.size() == 0)
						selectedGroupName = "";
				}
				ptfxEffects::saveEffects(savedEffects);

				groupToDelete             = "";
				customEffectToDeleteIndex = -1;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("No"))
			{
				groupToDelete             = "";
				customEffectToDeleteIndex = -1;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
}