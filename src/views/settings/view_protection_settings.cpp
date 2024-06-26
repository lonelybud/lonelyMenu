#include "core/data/protections.hpp"
#include "views/view.hpp"

namespace big
{
	void view::protection_settings()
	{
		ImGui::BeginGroup();
		ImGui::Checkbox("CEO Money", &g_protections.script_events.ceo_money);
		ImGui::Checkbox("Force Mission", &g_protections.script_events.force_mission);
		ImGui::Checkbox("Force Teleport", &g_protections.script_events.force_teleport);
		ImGui::Checkbox("GTA Banner", &g_protections.script_events.gta_banner);
		ImGui::Checkbox("MC Teleport", &g_protections.script_events.mc_teleport);
		ImGui::EndGroup();

		ImGui::SameLine();

		ImGui::BeginGroup();
		ImGui::Checkbox("Send to Cutscene", &g_protections.script_events.send_to_cutscene);
		ImGui::Checkbox("Send to Location", &g_protections.script_events.send_to_location);
		ImGui::Checkbox("Sound Spam Scripted", &g_protections.script_events.sound_spam);
		ImGui::Checkbox("Personal Vehicle Destroyed", &g_protections.script_events.personal_vehicle_destroyed);
		ImGui::Checkbox("Remote Off Radar", &g_protections.script_events.remote_off_radar);
		ImGui::Checkbox("Rotate Cam", &g_protections.script_events.rotate_cam);
		ImGui::EndGroup();

		ImGui::SameLine();

		ImGui::BeginGroup();
		ImGui::Checkbox("Teleport To Warehouse", &g_protections.script_events.teleport_to_warehouse);
		ImGui::Checkbox("Start Activity", &g_protections.script_events.start_activity);
		ImGui::Checkbox("Send SMS", &g_protections.script_events.send_sms);
		ImGui::Checkbox("Spectate", &g_protections.script_events.spectate);
		ImGui::Checkbox("Vehicle Kick", &g_protections.script_events.vehicle_kick);
		ImGui::Checkbox("Wanted Level", &g_protections.script_events.clear_wanted_level);
		ImGui::EndGroup();

		ImGui::SameLine();

		ImGui::BeginGroup();
		ImGui::Checkbox("Request Control Event", &g_protections.request_control_event); // teleporting you with vehicle
		ImGui::Checkbox("Clear Ped Tasks", &g_protections.clear_ped_tasks);             // teleporting you
		ImGui::Checkbox("Receive Pickup", &g_protections.receive_pickup);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("This prevents the collection of pickups such as unwanted money bags\nNote: Normal pickups are also no longer possible to collect with this enabled");
		ImGui::Checkbox("Block Traffic Manipulation", &g_protections.stop_traffic);
		ImGui::Checkbox("Cage Protection", &g_protections.cage_protection);
		ImGui::EndGroup();

		ImGui::SeparatorText("Options");
		ImGui::BeginGroup();
		if (ImGui::Button("Disable All Protections"))
			for (size_t i = (size_t)&g_protections; i <= (size_t) & (g_protections.receive_pickup); i++)
				*(bool*)i = false;
		ImGui::SameLine();
		if (ImGui::Button("Reset Protections"))
			g_protections = {};
		ImGui::EndGroup();
	};
}
