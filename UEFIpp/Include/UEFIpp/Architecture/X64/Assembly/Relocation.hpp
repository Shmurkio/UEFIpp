#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::Architecture::X64::Assembly
{
	enum class RelocationKind : Foundation::Uint8
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

	enum class RelocationTargetKind : Foundation::Uint8
	{
		InternalLabel,
		AbsoluteAddress
	};

	struct Relocation
	{
		Foundation::Size Offset{};
		RelocationKind Kind{};
		RelocationTargetKind TargetKind{};
		Foundation::Uint64 Target{};
		Foundation::Int64 Addend{};
	};
}
