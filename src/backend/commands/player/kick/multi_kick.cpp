#include "backend/player_command.hpp"
#include "script.hpp"
#include "services/recent_modders/recent_modders.hpp"

namespace big
{
	class multi_kick : player_command
	{
		using player_command::player_command;

		virtual CommandAccessLevel get_access_level() override
		{
			return CommandAccessLevel::TOXIC;
		}

		virtual void execute(player_ptr player, const command_arguments& _args, const std::shared_ptr<command_context> ctx) override
		{
			if (player && player->is_valid())
			{
				player->timeout();

				dynamic_cast<player_command*>(command::get(RAGE_JOAAT("endkick")))->call(player, {});
				script::get_current()->yield(250ms);
				dynamic_cast<player_command*>(command::get(RAGE_JOAAT("nfkick")))->call(player, {});

				if (auto net_data = player->get_net_data())
				{
					auto rockstar_id = net_data->m_gamer_handle.m_rockstar_id;
					if (!recent_modders_nm::does_exist(rockstar_id))
						recent_modders_nm::add_player({net_data->m_name, rockstar_id, true});
				};
			}
		}
	};

	multi_kick g_multi_kick("multikick", "Multi kick", "End, Null & OOM kick", 0, false);
}
