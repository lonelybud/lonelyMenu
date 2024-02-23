
#pragma once
#include <iostream>
#include <string>

namespace big
{
	// Function to trim leading and trailing whitespace from a string
	inline std::string trimString(const std::string& str)
	{
		auto start = str.begin();
		auto end   = str.end();

		// Find the first non-whitespace character from the start
		while (start != end && std::isspace(*start))
		{
			++start;
		}

		// Find the last non-whitespace character from the end
		while (start != end && std::isspace(*(end - 1)))
		{
			--end;
		}

		return std::string(start, end);
	}
}
