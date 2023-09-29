#include "natives.hpp"
#include "pointers.hpp"
#include "util/strings.hpp"
#include "views/view.hpp"

using json = nlohmann::json;

namespace big
{
	namespace play_audio
	{
		class Audio
		{
		public:
			std::string AudioName;
			std::string AudioRef;
		};
		class Saved_Audio : public Audio
		{
		public:
			std::string name;
		};

		NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Audio, AudioName, AudioRef);
		NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Saved_Audio, AudioName, AudioRef, name);

		void play_audio(Audio obj)
		{
			g_fiber_pool->queue_job([obj] {
				AUDIO::PLAY_SOUND_FROM_ENTITY(1, obj.AudioName.c_str(), self::ped, obj.AudioRef.c_str(), true, 0);
			});
		}

		std::filesystem::path getSavedAudiosFilePath()
		{
			return g_file_manager.get_project_file("savedAudios.json").get_path();
		}

		void load_audios(std::vector<Audio>& audios)
		{
			try
			{
				auto filePath = g_file_manager.get_project_file("soundNames.json").get_path();

				if (std::filesystem::exists(filePath))
				{
					std::ifstream f(filePath);

					if (f.is_open())
					{
						audios = json::parse(f);
						f.close();
					}
				}
				else
					g_notification_service->push_warning("Sorry", "Please download soundNames.json.");
			}
			catch (std::exception e)
			{
				LOG(WARNING) << e.what();
			}
		}
		void load_saved_audios(std::map<std::string, std::vector<Saved_Audio>>& audios)
		{
			auto savedFilePath = getSavedAudiosFilePath();
			audios.clear();
			try
			{
				if (std::filesystem::exists(savedFilePath))
				{
					std::ifstream f(savedFilePath);
					if (f.is_open())
					{
						audios = json::parse(f);
						f.close();
					}
				}
			}
			catch (std::exception e)
			{
				LOG(WARNING) << e.what();
			}
		}
		void save_audios(std::map<std::string, std::vector<Saved_Audio>>& audios)
		{
			auto savedFilePath = getSavedAudiosFilePath();
			try
			{
				json j = audios;

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

	void view::play_audio()
	{
		ImGui::SeparatorText("Play Audio");

		static std::vector<play_audio::Audio> audios;
		static std::map<std::string, std::vector<play_audio::Saved_Audio>> savedAudios;
		static play_audio::Audio selectedAudio;
		static std::string customAudioName, searchAudioText;

		components::button("Load audios", [&] {
			play_audio::load_audios(audios);
		});
		ImGui::SameLine();
		components::button("Stop sound", [&] {
			play_audio::play_audio({"SELECT", "HUD_FRONTEND_DEFAULT_SOUNDSET"});
		});

		ImGui::PushItemWidth(200);
		components::input_text_with_hint("##searchAudioText", "searchAudioText", searchAudioText);

		if (ImGui::BeginListBox("##audios", {200, static_cast<float>(*g_pointers->m_gta.m_resolution_y * 0.2)}))
		{
			auto objs = searchAudioText.length() > 0 ? filterStrings<play_audio::Audio>(audios,
			                searchAudioText,
			                [](play_audio::Audio& v) {
				                return v.AudioName;
			                }) :
			                                           audios;

			for (auto& obj : objs)
			{
				if (ImGui::Selectable(obj.AudioName.c_str(),
				        obj.AudioName == selectedAudio.AudioName && obj.AudioRef == selectedAudio.AudioRef))
				{
					selectedAudio   = obj;
					customAudioName = obj.AudioName;
					play_audio::play_audio(obj);
				}
			}

			ImGui::EndListBox();
		}

		static std::string groupName;

		ImGui::PushItemWidth(200);
		components::input_text_with_hint("##groupName", "groupName", groupName);
		ImGui::PushItemWidth(200);
		components::input_text_with_hint("##customAudioName", "customAudioName", customAudioName);
		components::button("save", [&] {
			std::string _customAudioName = trimString(customAudioName);
			std::string _groupName       = trimString(groupName);

			if (_customAudioName.size() > 0 && _groupName.size() > 0 && selectedAudio.AudioName.size())
			{
				play_audio::Saved_Audio a = {selectedAudio.AudioName, selectedAudio.AudioRef, _customAudioName};

				if (!savedAudios.size())
					play_audio::load_saved_audios(savedAudios);

				if (savedAudios.find(_groupName) != savedAudios.end())
					savedAudios[_groupName].push_back(a);
				else
					savedAudios[_groupName] = std::vector({a});

				play_audio::save_audios(savedAudios);
			}
		});
		ImGui::SameLine();
		components::button("Load saved audios", [&] {
			play_audio::load_saved_audios(savedAudios);
		});

		components::small_text("Shift click to delete a groupName or effect inside it");

		static std::string groupToDelete;
		static int customEffectToDeleteIndex = -1;

		ImGui::BeginGroup();
		components::small_text("groupNames");
		if (ImGui::BeginListBox("##groupNames", {200, static_cast<float>(*g_pointers->m_gta.m_resolution_y * 0.2)}))
		{
			for (auto& obj : savedAudios)
				if (ImGui::Selectable(obj.first.c_str(), obj.first == groupName))
					groupName = obj.first;
			ImGui::EndListBox();
		}
		ImGui::EndGroup();
		ImGui::SameLine();
		ImGui::BeginGroup();
		components::small_text("audioNames");
		if (groupName.length() && ImGui::BeginListBox("##audioNames", {200, static_cast<float>(*g_pointers->m_gta.m_resolution_y * 0.2)}))
		{
			for (auto& audio : savedAudios[groupName])
			{
				if (ImGui::Selectable(audio.name.c_str(), false))
					play_audio::play_audio(audio);
			}
			ImGui::EndListBox();
		}
		ImGui::EndGroup();
	}
}