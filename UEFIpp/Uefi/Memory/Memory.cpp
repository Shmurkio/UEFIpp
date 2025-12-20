#include "Memory.hpp"

static
inline
PVOID
Allocate(
	IN UINT64 Size
)
{
	if (!gBS || !Size)
	{
		return nullptr;
	}

	PVOID Memory = nullptr;
	EFI_STATUS Status = gBS->AllocatePool(EfiLoaderData, Size, &Memory);

	if (EfiError(Status))
	{
		return nullptr;
	}

	return Memory;
}

static
inline
VOID
Free(
	IN PVOID Memory
)
{
	if (!gBS || !Memory)
	{
		return;
	}

	gBS->FreePool(Memory);
}

PVOID
operator
new(
	IN CUINT64 Size
)
{
	return Allocate(Size);
}

PVOID
operator
new[](
	IN CUINT64 Size
)
{
	return Allocate(Size);
}

PVOID
operator
new(
	MAYBE_UNUSED IN CUINT64 Size,
	IN PVOID Address
)
{
	return Address;
}

PVOID
operator
new[](
	MAYBE_UNUSED IN CUINT64 Size,
	IN PVOID Address
)
{
	return Address;
}

VOID
operator
delete(
	IN PVOID Memory
)
{
	Free(Memory);
}

VOID
operator
delete[](
	IN PVOID Memory
)
{
	Free(Memory);
}

VOID
operator
delete(
	IN PVOID Memory,
	MAYBE_UNUSED IN UINT64 Size
)
{
	Free(Memory);
}

VOID
operator
delete[](
	IN PVOID Memory,
	MAYBE_UNUSED IN UINT64 Size
)
{
	Free(Memory);
}