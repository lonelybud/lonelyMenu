#include "core/enums.hpp"
#include "util/mobile.hpp"
#include "util/vehicle.hpp"
#include "views/view.hpp"

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
			vehicle::repair(vehicle::get_self_veh());
		});
	}

	void view::vehicle()
	{
		ImGui::Spacing();

		render_first_block();
	}
}
