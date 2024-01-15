#pragma once
#include "gta/joaat.hpp"

namespace big
{
	// Add new values to the bottom
	enum class Infraction
	{
		NONE,
		TRIGGERED_ANTICHEAT,
		TRIED_CRASH_PLAYER,
		TRIED_KICK_PLAYER,
		KILLED_WITH_GODMODE,
		KILLED_WITH_INVISIBILITY,
		KILLED_WHEN_HIDDEN_FROM_PLAYER_LIST,
		SPOOFED_DATA,
		KILLED_ORBITAL_CANON,
		SPOOFED_HOST_TOKEN,
		INVALID_PLAYER_MODEL,
		SUPER_JUMP,
		UNDEAD_OTR,
		PLAYED_YOU_POS,
		PLAYED_YOU_NEG,
		GIVE_COLLECTIBLE,
		CASH_SPAWN,
		REPORT,
		VOTE_KICK,
		REMOVE_WEAPON,
		GIVE_WEAPON,
		BOUNTY,
		IS_ROCKSTAR_DEV_OR_QA,
		IS_CHEATER,
	};

	inline std::unordered_map<Infraction, const char*> infraction_desc = {
	    {Infraction::TRIGGERED_ANTICHEAT, "Triggered Rockstar's anticheat"}, // notify once
	    {Infraction::TRIED_CRASH_PLAYER, "Tried to crash you"},              // kick
	    {Infraction::TRIED_KICK_PLAYER, "Tried to kick you"},                // kick
	    {Infraction::KILLED_WITH_GODMODE, "Killed someone while using godmode"},
	    {Infraction::KILLED_WITH_INVISIBILITY, "Killed someone while being invisible"},
	    {Infraction::KILLED_WHEN_HIDDEN_FROM_PLAYER_LIST, "Killed someone while hidden in player list"},
	    {Infraction::SPOOFED_DATA, "Had spoofed data"},                             // notify once
	    {Infraction::KILLED_ORBITAL_CANON, "Killed someone with orbital canon"},
	    {Infraction::SPOOFED_HOST_TOKEN, "Had spoofed their host token"},           // notify once
	    {Infraction::INVALID_PLAYER_MODEL, "Had used an invalid player model"},     // notify once
	    {Infraction::SUPER_JUMP, "Had used super jump"},                            // notify once
	    {Infraction::UNDEAD_OTR, "Had used undead OTR"},                            // notify once
	    {Infraction::PLAYED_YOU_POS, "Played with you positive"},                   // notify once
	    {Infraction::PLAYED_YOU_NEG, "Played with you negative"},                   // notify once
	    {Infraction::GIVE_COLLECTIBLE, "Tried to give collectible"},                // notify once
	    {Infraction::CASH_SPAWN, "Spawning cash"},                                  // notify once
	    {Infraction::REPORT, "Reported you"},
	    {Infraction::VOTE_KICK, "Trying to vote kick you"},
	    {Infraction::REMOVE_WEAPON, "Tried to remove weapon"}, // notify once
	    {Infraction::GIVE_WEAPON, "Tried to give weapon"},     // notify once
	    {Infraction::BOUNTY, "Set bounty on you"},
	    {Infraction::IS_ROCKSTAR_DEV_OR_QA, "Is a rockstar DEV or QA"}, // notify once
	    {Infraction::IS_CHEATER, "Is a cheater marked by rockstar"},    // notify once
	};
}