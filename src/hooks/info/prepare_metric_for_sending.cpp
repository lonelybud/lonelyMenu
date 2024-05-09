#include "gta/json_serializer.hpp"
#include "hooking/hooking.hpp"
#include "rage/rlMetric.hpp"
#include "services/players/player_service.hpp"

namespace big
{
	const auto bad_metrics = std::unordered_set<std::string_view>({
	    "REPORTER",
	    "REPORT_INVALIDMODEL",
	    "MEM_NEW",
	    "DEBUGGER_ATTACH",
	    "DIG",
	    "XP_LOSS",
	    // "AWARD_XP",
	    "CF",
	    "CC",
	    "CNR",
	    "SCRIPT",
	    "CHEAT",
	    "AUX_DEUX",
	    "WEATHER",
	    "HARDWARE_OS",
	    "HARDWARE_GPU",
	    "HARDWARE_MOBO",
	    "HARDWARE_MEM",
	    "HARDWARE_CPU",
	    "PCSETTINGS",
	    "CASH_CREATED",
	    "DR_PS",
	    // "UVC",
	    "W_L",
	    "ESVCS",
	    "IDLEKICK",
	    "GSCB",
	    "GSINV",
	    "GSCW",
	    "GSINT",
	    "EARN",
	    "GARAGE_TAMPER",
	    "DUPE_DETECT",
	    "LAST_VEH",
	    "FAIL_SERV",
	    "CCF_UPDATE",
	    "CODE_CRC",
	    "COLLECTIBLE",
	    "FIRST_VEH",
	    "MM",
	    "RDEV",
	    "RQA",
	    "RANK_UP",
	});

	std::string hex_encode(std::string_view input)
	{
		const char* hex_chars = "0123456789ABCDEF";
		std::string output;
		output.reserve(input.length() * 2); // Pre-allocate memory for efficiency
		for (unsigned char c : input)
		{
			output.push_back(hex_chars[c >> 4]);   // Extract the high nibble (4 bits)
			output.push_back(hex_chars[c & 0x0F]); // Extract the low nibble
		}
		return output;
	}

	std::string remove_module_from_mmlist(std::string_view input, std::string_view element_to_remove)
	{
		std::string result(input);
		std::string delimiter = "|";
		size_t start_pos      = 0;

		while (true)
		{
			size_t delimiter_pos = result.find(delimiter, start_pos);
			if (delimiter_pos == std::string::npos)
			{
				break;
			}
			std::string current_element = result.substr(start_pos, delimiter_pos - start_pos);
			if (current_element == element_to_remove)
			{
				result.erase(start_pos, delimiter_pos - start_pos + delimiter.length());
				break;
			}
			start_pos = delimiter_pos + delimiter.length();
		}

		return result;
	}

	bool hooks::prepare_metric_for_sending(rage::json_serializer* serializer, int unk, int time, rage::rlMetric* metric)
	{
		char metric_json_buffer[256]{};
		rage::json_serializer yim_serializer(metric_json_buffer, sizeof(metric_json_buffer));

		metric->serialize(&yim_serializer);

		const bool is_bad_metric = bad_metrics.contains(metric->get_name());

		if (is_bad_metric)
		{
			std::string data = yim_serializer.get_string();

			if (strcmp(metric->get_name(), "W_L") == 0)
				; // dont log W_L metric
			else
			{
				bool contains_your_sc = false;

				if (strcmp(metric->get_name(), "DIG") == 0)
					contains_your_sc = data.find(std::to_string(g_player_service->get_self()->m_rockstar_id)) != std::string::npos;

				LOG(WARNING) << "BAD METRIC: " << metric->get_name() << " " << contains_your_sc;
			}

			std::ofstream log(g_file_manager.get_project_file("./bad_metric.log").get_path(), std::ios::app);
			log << "BAD METRIC: " << metric->get_name() << "; DATA: " << data << std::endl;
			log.close();

			if (strcmp(metric->get_name(), "MM") == 0)
			{
				std::string data = std::string(reinterpret_cast<char*>(metric) + 0x18);
				char module_name[MAX_PATH];
				GetModuleFileNameA(g_hmodule, module_name, sizeof(module_name));
				std::string encoded_module_name = hex_encode(std::filesystem::path(module_name).filename().string());
				std::string result              = remove_module_from_mmlist(data, encoded_module_name + "00");
				if (result.size() != data.size())
					LOG(INFO) << "Removed YimMenu DLL from MM metric";
				strncpy(reinterpret_cast<char*>(metric) + 0x18, result.c_str(), 0x900);
			}
			else
				return false;
		}

		return g_hooking->get_original<prepare_metric_for_sending>()(serializer, unk, time, metric);
	}
}
