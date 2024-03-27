
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

	inline void strcpy_safe(char* dest, const char* src)
	{
		auto len = strlen(dest) - 1;
		strncpy(dest, src, len);
		if (strlen(src) > len)
			dest[len] = '\0';
	}
}
