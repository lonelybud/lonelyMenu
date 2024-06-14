#pragma once
#include "gta/enums.hpp"

namespace big
{
	const std::map<KickReason, const char*> kick_reasons = {
	    {KickReason::VOTED_OUT, "HOST KICK"},
	    {KickReason::PEER_COMPLAINTS, "PEER_COMPLAINTS"},
	    {KickReason::CONNECTION_ERROR, "CONNECTION_ERROR"},
	    {KickReason::NAT_TYPE, "NAT_TYPE"},
	    {KickReason::SCADMIN, "SCADMIN"},
	    {KickReason::SCADMIN_BLACKLIST, "SCADMIN_BLACKLIST"},
	    {KickReason::NUM_REASONS, "NUM_REASONS"},
	};
}