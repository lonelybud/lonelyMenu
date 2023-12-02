#pragma once

namespace big
{
	// needed for serialization of the pointers cache
#pragma pack(push, 1)
	struct socialclub_pointers
	{
		PVOID m_read_attribute_patch;
		PVOID m_read_attribute_patch_2;
	};
#pragma pack(pop)

	static_assert(sizeof(socialclub_pointers) % 8 == 0, "Pointers are not properly aligned");
}
