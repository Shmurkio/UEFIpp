#pragma once

#include "../Uefi.hpp"

#include "../Util/Util.hpp"

namespace NVRAM
{
	static inline constexpr CUINT64 gGetStaticCap = 1024;

	template <typename T>
	EFI_STATUS
	Get(
		IN CEFI_GUID& Guid,
		IN PCSTR Name,
		OUT T*& Data,
		OUT UINT64& DataSize,
		IN CBOOLEAN UseHeap
	)
	{
		Data = nullptr;
		DataSize = 0;

		// Validate parameters.
		if (!Name || !*Name)
		{
			return EFI_INVALID_PARAMETER;
		}

		// Convert name.
		PWSTR NameW = nullptr;
		PSTR NameTemp = const_cast<PSTR>(Name);
		EFI_STATUS Status = Util::StrToStrW(NameTemp, NameW, UseHeap, false);

		if (EfiError(Status))
		{
			return Status;
		}

		// Probe the required allocation size.
		UINT32 Attributes = 0;
		UINT64 Size = 0;

		Status = gRT->GetVariable(NameW, const_cast<PEFI_GUID>(&Guid), &Attributes, &Size, nullptr);

		if (Status != EFI_BUFFER_TOO_SMALL)
		{
			if (UseHeap)
			{
				delete[] NameW;
			}

			return Status;
		}

		// Allocate buffer/use static buffer for the variable's data.
		PUINT8 Out = nullptr;

		if (UseHeap)
		{
			Out = new UINT8[Size];

			if (!Out)
			{
				delete[] NameW;
				return EFI_OUT_OF_RESOURCES;
			}
		}
		else
		{
			if (Size > gGetStaticCap)
			{
				return EFI_BUFFER_TOO_SMALL;
			}

			static UINT8 Buffer[gGetStaticCap];

			Out = Buffer;
		}

		// Read into the allocated buffer.
		Status = gRT->GetVariable(NameW, const_cast<PEFI_GUID>(&Guid), &Attributes, &Size, Out);
		
		if (UseHeap)
		{
			delete[] NameW;
		}

		if (EfiError(Status))
		{
			if (UseHeap)
			{
				delete[] Out;
			}

			return Status;
		}

		Data = reinterpret_cast<T*>(Out);
		DataSize = Size;

		return EFI_SUCCESS;
	}
}