#include "matchmaking_service.hpp"

#include "core/data/session.hpp"
#include "fiber_pool.hpp"
#include "hooking/hooking.hpp"
#include "script.hpp"

#include <network/Network.hpp>

namespace big
{
	matchmaking_service::matchmaking_service()
	{
		g_matchmaking_service = this;
	}

	matchmaking_service::~matchmaking_service()
	{
		g_matchmaking_service = nullptr;
	}

	bool matchmaking_service::handle_advertise(int num_slots, int available_slots, rage::rlSessionInfo* info, MatchmakingAttributes* attributes, MatchmakingId* out_id, rage::rlTaskStatus* status)
	{
		if (!g_session.multiplex_session)
			return false;

		if (status->status)
			return true;

		status->status = 1; // set in progress

		// create the first advertisement
		g_fiber_pool->queue_job([this, num_slots, available_slots, info, attributes, out_id, status] {
			rage::rlTaskStatus our_status{};
			if (!g_hooking->get_original<hooks::advertise_session>()(0, num_slots, available_slots, attributes, -1, info, out_id, &our_status))
			{
				LOG(WARNING) << __FUNCTION__ ": advertise_session returned false for first advertisement";
				status->status = 2;
				return;
			}

			while (our_status.status == 1)
				script::get_current()->yield();

			if (our_status.status == 2)
			{
				LOG(WARNING) << __FUNCTION__ ": advertise_session failed for first advertisement";
				status->status = 2;
				return;
			}

			MatchmakingId primary_id = *out_id; // create a copy if the original memory gets deallocated
			std::uint32_t id_hash    = rage::joaat(primary_id.m_data1);

			// m_data1 is generated from m_data2 and m_data3
			m_multiplexed_sessions.emplace(id_hash, std::vector<MatchmakingId>{});

			// create multiplex advertisements
			for (int i = 0; i < (g_session.multiplex_count - 1); i++)
			{
				g_fiber_pool->queue_job([this, num_slots, available_slots, info, attributes, id_hash, i] {
					rage::rlTaskStatus status;
					MatchmakingId multiplexed_id;
					if (!g_hooking->get_original<hooks::advertise_session>()(0, num_slots, available_slots, attributes, -1, info, &multiplexed_id, &status))
					{
						LOG(WARNING) << __FUNCTION__ ": advertise_session returned false for multiplex task " << i;
						return;
					}

					while (status.status == 1)
						script::get_current()->yield();

					if (status.status == 2)
					{
						LOG(WARNING) << __FUNCTION__ ": advertise_session failed for multiplex task " << i;
						return;
					}

					if (auto it = m_multiplexed_sessions.find(id_hash); it != m_multiplexed_sessions.end())
					{
						it->second.push_back(multiplexed_id);
					}
					else
					{
						LOG(WARNING) << __FUNCTION__ ": created a multiplexed session advertisement, but the primary advertisement no longer exists";
					}
				});
			}
			status->status = 3; // return success for original caller
		});

		return true;
	}

	void matchmaking_service::handle_update(int num_slots, int available_slots, rage::rlSessionInfo* info, MatchmakingAttributes* attributes, MatchmakingId* id)
	{
		// this can be fire and forget, but it's probably a good idea to be notified if something goes wrong
		if (auto it = m_multiplexed_sessions.find(rage::joaat(id->m_data1)); it != m_multiplexed_sessions.end())
		{
			if (!g_session.multiplex_session)
			{
				// option disabled mid-session
				return;
			}

			int i = 0;
			for (auto& multiplex_session : it->second)
			{
				g_fiber_pool->queue_job([&multiplex_session, num_slots, available_slots, info, attributes, i] {
					rage::rlTaskStatus status;
					if (!g_hooking->get_original<hooks::update_session_advertisement>()(0, &multiplex_session, num_slots, available_slots, info, attributes, &status))
					{
						LOG(WARNING) << __FUNCTION__ ": update_session_advertisement returned false for multiplex task " << i;
						return;
					}

					while (status.status == 1)
						script::get_current()->yield();

					if (status.status == 2)
					{
						LOG(WARNING) << __FUNCTION__ ": update_session_advertisement failed for multiplex task " << i;
						return;
					}
				});
				i++;
			}
		}
	}

	bool matchmaking_service::handle_unadvertise(MatchmakingId* id)
	{
		if (auto it = m_multiplexed_sessions.find(rage::joaat(id->m_data1)); it != m_multiplexed_sessions.end())
		{
			for (auto& multiplex_session : it->second)
			{
				g_hooking->get_original<hooks::unadvertise_session>()(0, &multiplex_session, nullptr);
			}

			m_multiplexed_sessions.erase(it);
			return false;
		}
		else
		{
			if (g_session.multiplex_session)
			{
				for (auto& [_, children] : m_multiplexed_sessions)
				{
					for (auto& session : children)
					{
						if (session.m_data2 == id->m_data2 && session.m_data3 == id->m_data3)
						{
							return true; // prevent auto cleanup
						}
					}
				}
			}
		}

		return false;
	}
}