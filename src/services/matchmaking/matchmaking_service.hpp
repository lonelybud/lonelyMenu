#pragma once

#include <network/MatchmakingId.hpp>
#include <rage/rlTaskStatus.hpp>

namespace rage
{
	class rlSessionInfo;
	class rlSessionDetailMsg;
}

class MatchmakingAttributes;

namespace big
{
	class matchmaking_service
	{
	private:
		std::unordered_map<std::uint32_t, std::vector<MatchmakingId>> m_multiplexed_sessions;

		void patch_matchmaking_attributes(MatchmakingAttributes* attributes);

	public:
		matchmaking_service();
		~matchmaking_service();

		bool handle_advertise(int num_slots, int available_slots, rage::rlSessionInfo* info, MatchmakingAttributes* attributes, MatchmakingId* out_id, rage::rlTaskStatus* status);
		void handle_update(int num_slots, int available_slots, rage::rlSessionInfo* info, MatchmakingAttributes* attributes, MatchmakingId* id);
		bool handle_unadvertise(MatchmakingId* id);
		void handle_session_detail_send_response(rage::rlSessionDetailMsg* msg);
	};

	inline matchmaking_service* g_matchmaking_service;
}