#include "core/enums.hpp"
#include "util/mobile.hpp"
#include "util/vehicle.hpp"
#include "views/view.hpp"

constexpr int wheelIndexes[4]{0, 1, 4, 5};
constexpr auto MAX_VEHICLE_DOORS = 6;
const char* const doornames[MAX_VEHICLE_DOORS]{
    "D_1",
    "D_2",
    "D_3",
    "D_4",
    "Bon",
    "Trk",
};

namespace big
{
	static inline void render_first_block()
	{
		components::button("Mors Mutual Fix All Vehicles", [] {
			int amount_fixed = mobile::mors_mutual::fix_all();
			g_notification_service->push_success("Mobile", std::vformat("{} vehicle{} been fixed.", std::make_format_args(amount_fixed, amount_fixed == 1 ? " has" : "s have")));
		});
		ImGui::SameLine();
		components::button("Repair", [] {
			vehicle::repair();
		});
	}

	static inline void render_fun_feats()
	{
		static float maxWheelRaiseFactor = 0;
		static int no_of_seats           = 0;

		if (self::veh)
		{
			if (no_of_seats == 0)
			{
				no_of_seats = -1;
				g_fiber_pool->queue_job([] {
					no_of_seats = VEHICLE::GET_VEHICLE_MAX_NUMBER_OF_PASSENGERS(self::veh) + 1;
				});
			}

			ImGui::Text(std::format("No of seat: {}", no_of_seats).c_str());

			ImGui::Spacing();

			ImGui::BeginGroup();
			{
				components::sub_title("Basic Controls");

				components::small_text("Doors");
				{
					for (int i = 0; i < MAX_VEHICLE_DOORS; i++)
					{
						components::button(doornames[i], [=] {
							vehicle::operate_vehicle_door(self::veh, (eDoorId)i, !(VEHICLE::GET_VEHICLE_DOOR_ANGLE_RATIO(self::veh, i) > 0.0f));
						});
						ImGui::SameLine();
					}
					ImGui::NewLine();
				}
			}
			ImGui::EndGroup();
			ImGui::SameLine(0, 2.0f * ImGui::GetTextLineHeight());
			ImGui::BeginGroup();
			{
				ImGui::BeginGroup();
				{
					components::sub_title("Lowrider Controls");

					components::button("Get suspension raise factor", [&] {
						maxWheelRaiseFactor = VEHICLE::GET_HYDRAULIC_SUSPENSION_RAISE_FACTOR(self::veh, 0);
					});

					ImGui::Spacing();

					if (maxWheelRaiseFactor)
					{
						for (int i = 0; i < 4; ++i)
						{
							components::button("Raise W" + std::to_string(i + 1), [&, wheelIndex = wheelIndexes[i]] {
								bool raised = VEHICLE::GET_HYDRAULIC_SUSPENSION_RAISE_FACTOR(self::veh, wheelIndex) < maxWheelRaiseFactor ? false : true;
								if (raised)
									VEHICLE::SET_HYDRAULIC_WHEEL_STATE(self::veh, wheelIndex, 0, maxWheelRaiseFactor, 1);
								else
								{
									VEHICLE::SET_HYDRAULIC_WHEEL_STATE(self::veh, wheelIndex, 4, maxWheelRaiseFactor, 1);
									script::get_current()->yield(250ms);
									VEHICLE::SET_HYDRAULIC_WHEEL_STATE(self::veh, wheelIndex, 1, maxWheelRaiseFactor, 1);
								}
							});
							ImGui::SameLine();
						}
						ImGui::NewLine();

						components::button("Raise all", [&] {
							VEHICLE::SET_HYDRAULIC_WHEEL_STATE(self::veh, 0, 4, maxWheelRaiseFactor, 0);
							VEHICLE::SET_HYDRAULIC_WHEEL_STATE(self::veh, 1, 4, maxWheelRaiseFactor, 0);
							VEHICLE::SET_HYDRAULIC_WHEEL_STATE(self::veh, 4, 4, maxWheelRaiseFactor, 0);
							VEHICLE::SET_HYDRAULIC_WHEEL_STATE(self::veh, 5, 4, maxWheelRaiseFactor, 0);

							VEHICLE::SET_HYDRAULIC_WHEEL_STATE(self::veh, 0, 1, maxWheelRaiseFactor, 0);
							VEHICLE::SET_HYDRAULIC_WHEEL_STATE(self::veh, 1, 1, maxWheelRaiseFactor, 0);
							VEHICLE::SET_HYDRAULIC_WHEEL_STATE(self::veh, 4, 1, maxWheelRaiseFactor, 0);
							VEHICLE::SET_HYDRAULIC_WHEEL_STATE(self::veh, 5, 1, maxWheelRaiseFactor, 0);
						});
						ImGui::SameLine();
						components::button("Lower all", [&] {
							VEHICLE::SET_HYDRAULIC_VEHICLE_STATE(self::veh, 0);
						});
					}
				}
				ImGui::EndGroup();
			}
			ImGui::EndGroup();
		}
		else
		{
			components::small_text("Please sit in a vehicle");
			maxWheelRaiseFactor = 0;
			no_of_seats         = 0;
		}
	}

	static inline void render_general()
	{
		components::command_checkbox<"vehgodmode">();

		components::command_checkbox<"veh_boost">();

		components::command_checkbox<"rocketability">();

		static bool can_be_knocked_off_veh = true;
		components::button(can_be_knocked_off_veh ? "Enable Seat Belt" : "Disable Seat Belt", [] {
			if (can_be_knocked_off_veh)
			{
				PED::SET_PED_CONFIG_FLAG(self::ped, 32, 0);
				PED::SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(self::ped, (int)eKnockOffVehicle::KNOCKOFFVEHICLE_NEVER);
			}
			else
			{
				PED::SET_PED_CONFIG_FLAG(self::ped, 32, 1);
				PED::SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(self::ped, (int)eKnockOffVehicle::KNOCKOFFVEHICLE_DEFAULT);
			}

			can_be_knocked_off_veh = !can_be_knocked_off_veh;
		});

		components::sub_title("set/get weapon capacity");

		static int weapon_index, capacity;

		ImGui::SetNextItemWidth(100);
		ImGui::InputInt("Weapon Index", &weapon_index);
		ImGui::SameLine();
		components::button("Get ammo capacity", [] {
			auto c = VEHICLE::GET_VEHICLE_WEAPON_RESTRICTED_AMMO(self::veh, weapon_index);
			LOG(INFO) << "VEHICLE::GET_VEHICLE_WEAPON_RESTRICTED_AMMO = " << c;
		});

		ImGui::SetNextItemWidth(100);
		ImGui::InputInt("Capacity", &capacity);
		ImGui::SameLine();
		components::button("Set ammo capacity", [] {
			VEHICLE::SET_VEHICLE_WEAPON_RESTRICTED_AMMO(self::veh, weapon_index, capacity);
		});
	}

	void view::vehicle()
	{
		ImGui::Spacing();

		render_first_block();

		ImGui::SeparatorText("###General");

		render_general();

		ImGui::SeparatorText("Fun###funFeatures");

		render_fun_feats();
	}
}
