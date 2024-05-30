#pragma once

namespace big
{
	struct RegionType
	{
		uint32_t id;
		const char name[22];
	};

	inline const RegionType regions[] = {
	    {0, "CIS"},
	    {1, "Africa"},
	    {2, "East"},
	    {3, "Europe"},
	    {4, "China"},
	    {5, "Australia"},
	    {6, "West"},
	    {7, "Japan"},
	    {8, "Unknown"},
	};

	inline const RegionType nat_types[] = {
	    {0, "Zero"},
	    {1, "open"},
	    {2, "moderate"},
	    {3, "strict"},
	    {4, "unkown"},
	};
}