#pragma once

#include "../Uefi.hpp"

PVOID
operator
new(
	IN CUINT64 Size
);

PVOID
operator
new[](
	IN CUINT64 Size
);

PVOID
operator
new(
	MAYBE_UNUSED IN CUINT64 Size,
	IN PVOID Address
);

PVOID
operator
new[](
	MAYBE_UNUSED IN CUINT64 Size,
	IN PVOID Address
);

VOID
operator
delete(
	IN PVOID Memory
);

VOID
operator
delete[](
	IN PVOID Memory
);

VOID
operator
delete(
	IN PVOID Memory,
	MAYBE_UNUSED IN UINT64 Size
);

VOID
operator
delete[](
	IN PVOID Memory,
	MAYBE_UNUSED IN UINT64 Size
);