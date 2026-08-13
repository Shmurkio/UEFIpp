#pragma once

#include <UEFIpp/Architecture/X64/Assembly/Label.hpp>

namespace UEFIpp::Architecture::X64::Assembly
{
	enum class FixupKind : Foundation::Uint8
	{
		Relative8,
		Relative16,
		Relative32,
		Relative64,
		RipRelative32,
		Absolute8,
		Absolute16,
		Absolute32,
		Absolute64
	};

	struct Fixup
	{
		Foundation::Size Offset{};
		FixupKind Kind{};
		Label Target{};
		Foundation::Int64 Addend{};
	};
}
