#include "natives.hpp"
#include "pointers.hpp"
#include "services/context_menu/context_menu_service.hpp"
#include "services/notifications/notification_service.hpp"
#include "util/strings.hpp"
#include "views/view.hpp"

using json = nlohmann::json;

const char* speechParams[] = {"Speech_Params_Standard", "Speech_Params_Allow_Repeat", "Speech_Params_Beat", "Speech_Params_Force", "Speech_Params_Force_Frontend", "Speech_Params_Force_No_Repeat_Frontend", "Speech_Params_Force_Normal", "Speech_Params_Force_Normal_Clear", "Speech_Params_Force_Normal_Critical", "Speech_Params_Force_Shouted", "Speech_Params_Force_Shouted_Clear", "Speech_Params_Force_Shouted_Critical", "Speech_Params_Force_Preload_Only", "Speech_Params_Megaphone", "Speech_Params_Heli", "Speech_Params_Force_Megaphone", "Speech_Params_Force_Heli", "Speech_Params_Interrupt", "Speech_Params_Interrupt_Shouted", "Speech_Params_Interrupt_Shouted_Clear", "Speech_Params_Interrupt_Shouted_Critical", "Speech_Params_Interrupt_No_Force", "Speech_Params_Interrupt_Frontend", "Speech_Params_Interrupt_No_Force_Frontend", "Speech_Params_Add_Blip", "Speech_Params_Add_Blip_Allow_Repeat", "Speech_Params_Add_Blip_Force", "Speech_Params_Add_Blip_Shouted", "Speech_Params_Add_Blip_Shouted_Force", "Speech_Params_Add_Blip_Interrupt", "Speech_Params_Add_Blip_Interrupt_Force", "Speech_Params_Force_Preload_Only_Shouted", "Speech_Params_Force_Preload_Only_Shouted_Clear", "Speech_Params_Force_Preload_Only_Shouted_Critical", "Speech_Params_Shouted", "Speech_Params_Shouted_Clear", "Speech_Params_Shouted_Critical"};

namespace big
{
	namespace play_voice
	{
		struct Speech
		{
			std::string Name;
			unsigned short Index;
		};

		class Voice
		{
		public:
			std::string Name;
			std::vector<Speech> Speeches;
		};

		NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Speech, Name, Index);
		NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Voice, Name, Speeches);

		void load_voices(std::vector<Voice>& voices)
		{
			try
			{
				auto filePath = g_file_manager.get_project_file("speeches.json").get_path();

				if (std::filesystem::exists(filePath))
				{
					std::ifstream f(filePath);

					if (f.is_open())
					{
						voices = json::parse(f);
						f.close();
					}
				}
				else
					g_notification_service->push_warning("Sorry", "Please download speeches.json.");
			}
			catch (std::exception e)
			{
				LOG(WARNING) << e.what();
			}
		}

		inline void play(Entity e, const std::function<void()>& callback)
		{
			if (ENTITY::DOES_ENTITY_EXIST(e) && !ENTITY::IS_ENTITY_DEAD(e, true))
				g_fiber_pool->queue_job([=] {
					callback();
				});
		}
	}

	void view::play_voice()
	{
		ImGui::BeginGroup();

		ImGui::SeparatorText("Play Voice");

		static std::vector<play_voice::Voice> voices;
		static play_voice::Voice selectedVoice;
		static std::vector<std::string> speeches;
		static std::string searchVoiceText, searchSpeechText, selectedSpeech;
		static const char* selectedSpeechParam = "Speech_Params_Standard";

		ImGui::BeginGroup();
		if (ImGui::BeginListBox("##speechParamList", {400, static_cast<float>(*g_pointers->m_gta.m_resolution_y * 0.2)}))
		{
			for (auto& speechParam : speechParams)
				if (ImGui::Selectable(speechParam, speechParam == selectedSpeechParam))
					selectedSpeechParam = speechParam;

			ImGui::EndListBox();
		}
		ImGui::EndGroup();
		ImGui::Spacing();
		components::button("Load Voices", [&] {
			play_voice::load_voices(voices);
		});

		ImGui::BeginGroup();
		ImGui::PushItemWidth(200);
		components::input_text_with_hint("##searchVoiceText", "searchVoiceText", searchVoiceText);
		ImGui::PopItemWidth();
		if (ImGui::BeginListBox("##voiceList", {200, static_cast<float>(*g_pointers->m_gta.m_resolution_y * 0.2)}))
		{
			auto objs = searchVoiceText.length() > 0 ? filterStrings<play_voice::Voice>(voices,
			                searchVoiceText,
			                [](play_voice::Voice& v) {
				                return v.Name;
			                }) :
			                                           voices;

			for (auto& obj : objs)
				if (ImGui::Selectable(obj.Name.c_str(), obj.Name == selectedVoice.Name))
				{
					selectedVoice = obj;

					speeches.clear();
					for (auto speech : obj.Speeches)
						// Check if the speech Name is already in the speeches vector
						if (std::find(speeches.begin(), speeches.end(), speech.Name) == speeches.end())
							speeches.push_back(speech.Name); // Add unique speech Name to the vector
				}


			ImGui::EndListBox();
		}
		ImGui::EndGroup();
		ImGui::SameLine();
		ImGui::BeginGroup();
		ImGui::PushItemWidth(200);
		components::input_text_with_hint("##searchSpeechText", "searchSpeechText", searchSpeechText);
		ImGui::PopItemWidth();
		if (ImGui::BeginListBox("##speechList", {200, static_cast<float>(*g_pointers->m_gta.m_resolution_y * 0.2)}))
		{
			auto objs = searchSpeechText.length() > 0 ? filterStrings(speeches, searchSpeechText) : speeches;

			for (auto& s : objs)
				if (ImGui::Selectable(s.c_str(), s == selectedSpeech))
					selectedSpeech = s;

			ImGui::EndListBox();
		}
		ImGui::EndGroup();

		ImGui::SeparatorText("Play Speech");

		if (selectedSpeech.length())
		{
			auto m_handle  = g_context_menu_service->m_handle;
			auto m_pointer = g_context_menu_service->m_pointer;

			if (m_handle && m_pointer && m_pointer->m_model_info->m_model_type == eModelType::Ped)
			{
				components::small_text("PLAY_PED_AMBIENT_SPEECH_NATIVE(ped, speechName, speechParam, p3)");
				components::button("play###PLAY_PED_AMBIENT_SPEECH_NATIVE", [=] {
					auto myLambda = [=]() {
						AUDIO::PLAY_PED_AMBIENT_SPEECH_NATIVE(m_handle, selectedSpeech.c_str(), selectedSpeechParam, true);
					};
					play_voice::play(m_handle, myLambda);
				});
				ImGui::Spacing();
				components::small_text("PLAY_PED_AMBIENT_SPEECH_AND_CLONE_NATIVE(ped, speechName, speechParam, p3)");
				components::button("play###PLAY_PED_AMBIENT_SPEECH_AND_CLONE_NATIVE", [=] {
					auto myLambda = [=]() {
						AUDIO::PLAY_PED_AMBIENT_SPEECH_AND_CLONE_NATIVE(m_handle, selectedSpeech.c_str(), selectedSpeechParam, true);
					};
					play_voice::play(m_handle, myLambda);
				});
				ImGui::Spacing();
				components::small_text("PLAY_PED_AMBIENT_SPEECH_WITH_VOICE_NATIVE(ped, speechName, voiceName, speechParam, p4)");
				components::button("play###PLAY_PED_AMBIENT_SPEECH_WITH_VOICE_NATIVE", [=] {
					auto myLambda = [=]() {
						AUDIO::PLAY_PED_AMBIENT_SPEECH_WITH_VOICE_NATIVE(m_handle,
						    selectedSpeech.c_str(),
						    selectedVoice.Name.c_str(),
						    selectedSpeechParam,
						    true);
					};
					play_voice::play(m_handle, myLambda);
				});
				ImGui::Spacing();
				components::small_text("PLAY_AMBIENT_SPEECH_FROM_POSITION_NATIVE(speechName, voiceName, x, y, z, speechParam)");
				components::button("play###PLAY_AMBIENT_SPEECH_FROM_POSITION_NATIVE", [=] {
					auto myLambda = [=]() {
						AUDIO::PLAY_AMBIENT_SPEECH_FROM_POSITION_NATIVE(selectedSpeech.c_str(),
						    selectedVoice.Name.c_str(),
						    self::pos.x,
						    self::pos.y,
						    self::pos.z,
						    selectedSpeechParam);
					};
					play_voice::play(m_handle, myLambda);
				});
				ImGui::SameLine();
				components::small_text("This one play at player coordinates");
			}
			else
				components::small_text("Please select a valid ped using context menu");
		}
		else
			components::small_text("Please select a speech");


		ImGui::EndGroup();
	}
}