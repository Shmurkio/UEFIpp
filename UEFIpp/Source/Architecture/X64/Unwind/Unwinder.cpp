#include <UEFIpp/Architecture/X64/Unwind/Unwinder.hpp>

#include <UEFIpp/Executable/Pe/Image.hpp>

namespace
{
	using namespace UEFIpp;
	using namespace UEFIpp::Architecture::X64::Unwind;

	enum class UnwindOperation : Foundation::Uint8
	{
		PushNonvolatile = 0,
		AllocateLarge = 1,
		AllocateSmall = 2,
		SetFramePointer = 3,
		SaveNonvolatile = 4,
		SaveNonvolatileFar = 5,
		SaveXmm128 = 8,
		SaveXmm128Far = 9,
		PushMachineFrame = 10
	};

	inline constexpr auto ChainedInfoFlag = Foundation::Uint8{ 0x04 };

	struct UnwindInfoHeader
	{
		Foundation::Uint8 VersionAndFlags;
		Foundation::Uint8 PrologSize;
		Foundation::Uint8 CodeCount;
		Foundation::Uint8 FrameRegisterAndOffset;

		[[nodiscard]] constexpr auto Version() const -> Foundation::Uint8
		{
			return VersionAndFlags & 0x07;
		}

		[[nodiscard]] constexpr auto Flags() const -> Foundation::Uint8
		{
			return VersionAndFlags >> 3;
		}

		[[nodiscard]] constexpr auto FrameRegister() const -> Foundation::Uint8
		{
			return FrameRegisterAndOffset & 0x0F;
		}

		[[nodiscard]] constexpr auto FrameOffset() const -> Foundation::Uint8
		{
			return FrameRegisterAndOffset >> 4;
		}
	};

	struct UnwindCode
	{
		Foundation::Uint8 CodeOffset;
		Foundation::Uint8 OperationAndInfo;

		[[nodiscard]] constexpr auto Operation() const -> UnwindOperation
		{
			return Foundation::Cast::Auto<UnwindOperation>(
				OperationAndInfo & 0x0F
			);
		}

		[[nodiscard]] constexpr auto OperationInfo() const -> Foundation::Uint8
		{
			return OperationAndInfo >> 4;
		}
	};

	[[nodiscard]] constexpr auto AlignedCodeCount(
		Foundation::Size Count
	) -> Foundation::Size
	{
		return (Count + 1) & ~Foundation::Size{ 1 };
	}

	[[nodiscard]] constexpr auto RangeValid(
		Foundation::Size Offset,
		Foundation::Size Size,
		Foundation::Size Limit
	) -> Foundation::Bool
	{
		return Offset <= Limit && Size <= Limit - Offset;
	}


	[[nodiscard]] constexpr auto OperationSlotCount(
		const UnwindCode& Code
	) -> Foundation::Size
	{
		switch (Code.Operation())
		{
		case UnwindOperation::AllocateLarge:
			return Code.OperationInfo() == 1 ? 3 : 2;
		case UnwindOperation::SaveNonvolatile:
		case UnwindOperation::SaveXmm128:
			return 2;
		case UnwindOperation::SaveNonvolatileFar:
		case UnwindOperation::SaveXmm128Far:
			return 3;
		default:
			return 1;
		}
	}

	[[nodiscard]] auto ReadUint16(
		const UnwindCode* Codes,
		Foundation::Size Index
	) -> Foundation::Uint16
	{
		return Foundation::Cast::Auto<Foundation::Uint16>(Codes[Index].CodeOffset) |
			Foundation::Cast::Auto<Foundation::Uint16>(
				Foundation::Cast::Auto<Foundation::Uint16>(Codes[Index].OperationAndInfo) << 8
			);
	}

	[[nodiscard]] auto ReadUint32(
		const UnwindCode* Codes,
		Foundation::Size Index
	) -> Foundation::Uint32
	{
		return Foundation::Cast::Auto<Foundation::Uint32>(ReadUint16(Codes, Index)) |
			(Foundation::Cast::Auto<Foundation::Uint32>(ReadUint16(Codes, Index + 1)) << 16);
	}
}

struct UEFIpp::Architecture::X64::Unwind::Unwinder::RuntimeFunction
{
	Foundation::Uint32 BeginAddress;
	Foundation::Uint32 EndAddress;
	Foundation::Uint32 UnwindInfoAddress;
};

UEFIpp::Architecture::X64::Unwind::Unwinder::Unwinder(
	const Foundation::Void* ImageBase
)
{
	const Executable::Pe::Image Image{ ImageBase };
	if (!Image.IsValid() || !Image.Is64())
	{
		return;
	}

	const auto Directory = Image.GetDirectory(
		Executable::Pe::DirectoryIndex::Exception
	);

	if (Directory.Empty() ||
		(Directory.Size() % sizeof(RuntimeFunction)) != 0)
	{
		return;
	}

	const auto ImageSize = Foundation::Cast::Auto<Foundation::Size>(
		Image.ImageSize()
	);

	const auto DirectoryRva = Foundation::Cast::Auto<Foundation::Size>(
		Directory.Rva()
	);

	const auto DirectorySize = Foundation::Cast::Auto<Foundation::Size>(
		Directory.Size()
	);

	if (!RangeValid(DirectoryRva, DirectorySize, ImageSize))
	{
		return;
	}

	Base_ = Foundation::Cast::Auto<const Foundation::Uint8*>(ImageBase);
	ImageSize_ = ImageSize;
	Functions_ = Foundation::Cast::Auto<const RuntimeFunction*>(
		Base_ + DirectoryRva
	);
	FunctionCount_ = DirectorySize / sizeof(RuntimeFunction);
}

auto UEFIpp::Architecture::X64::Unwind::Unwinder::Valid() const
-> Foundation::Bool
{
	return Base_ && ImageSize_ && Functions_ && FunctionCount_;
}

auto UEFIpp::Architecture::X64::Unwind::Unwinder::Contains(
	Foundation::UintPtr Address
) const -> Foundation::Bool
{
	if (!Base_ || !ImageSize_)
	{
		return false;
	}

	const auto Base = Foundation::Cast::Auto<Foundation::UintPtr>(Base_);
	return Address >= Base && Address - Base < ImageSize_;
}

auto UEFIpp::Architecture::X64::Unwind::Unwinder::ImageBase() const
-> Foundation::UintPtr
{
	return Foundation::Cast::Auto<Foundation::UintPtr>(Base_);
}

auto UEFIpp::Architecture::X64::Unwind::Unwinder::ImageSize() const
-> Foundation::Size
{
	return ImageSize_;
}

auto UEFIpp::Architecture::X64::Unwind::Unwinder::FindRuntimeFunction(
	Foundation::UintPtr Rip
) const -> Library::Optional<RuntimeFunctionRange>
{
	const auto* Function = LookupRuntimeFunction(Rip);
	if (!Function)
	{
		return {};
	}

	const auto Base = ImageBase();
	return RuntimeFunctionRange
	{
		Base + Function->BeginAddress,
		Base + Function->EndAddress,
		Function->BeginAddress,
		Function->EndAddress,
		Function->UnwindInfoAddress
	};
}

auto UEFIpp::Architecture::X64::Unwind::Unwinder::LookupRuntimeFunction(
	Foundation::UintPtr Rip
) const -> const RuntimeFunction*
{
	if (!Valid() || !Contains(Rip))
	{
		return nullptr;
	}

	const auto Rva = Foundation::Cast::Auto<Foundation::Uint32>(
		Rip - ImageBase()
	);

	Foundation::Size First{};
	auto Count = FunctionCount_;

	while (Count)
	{
		const auto Step = Count / 2;
		const auto Index = First + Step;
		const auto& Function = Functions_[Index];

		if (Rva < Function.BeginAddress)
		{
			Count = Step;
		}
		else if (Rva >= Function.EndAddress)
		{
			First = Index + 1;
			Count -= Step + 1;
		}
		else
		{
			return &Function;
		}
	}

	return nullptr;
}

auto UEFIpp::Architecture::X64::Unwind::Unwinder::Register(
	Context& Value,
	Foundation::Uint8 Id
) const -> Library::Expected<Foundation::UintPtr*, UnwindError>
{
	switch (Id)
	{
	case 3:
		return &Value.Rbx;
	case 5:
		return &Value.Rbp;
	case 6:
		return &Value.Rsi;
	case 7:
		return &Value.Rdi;
	case 12:
		return &Value.R12;
	case 13:
		return &Value.R13;
	case 14:
		return &Value.R14;
	case 15:
		return &Value.R15;
	default:
		return Library::Unexpected{ UnwindError::UnsupportedRegister };
	}
}

auto UEFIpp::Architecture::X64::Unwind::Unwinder::VirtualUnwind(
	const Context& Current
) const -> Library::Expected<Frame, UnwindError>
{
	if (!Valid() || !Current.Rip || !Current.Rsp || !Contains(Current.Rip))
	{
		return Library::Unexpected{ UnwindError::InvalidContext };
	}

	auto Caller = Current;
	const auto* Function = LookupRuntimeFunction(Current.Rip);

	if (!Function)
	{
		auto* ReturnAddressSlot = Foundation::Cast::Auto<Foundation::UintPtr*>(
			Caller.Rsp
		);

		Caller.Rip = *ReturnAddressSlot;
		Caller.Rsp += sizeof(Foundation::UintPtr);

		return Frame{ Caller, ReturnAddressSlot };
	}

	const RuntimeFunction* CurrentFunction = Function;
	auto ControlRva = Foundation::Cast::Auto<Foundation::Uint32>(
		Current.Rip - ImageBase()
	);
	auto FirstInfo = true;

	for (;;)
	{
		if (!CurrentFunction->UnwindInfoAddress ||
			CurrentFunction->UnwindInfoAddress >= ImageSize_)
		{
			return Library::Unexpected{ UnwindError::InvalidRuntimeFunction };
		}

		const auto* Header = Foundation::Cast::Auto<const UnwindInfoHeader*>(
			Base_ + CurrentFunction->UnwindInfoAddress
		);

		if (Header->Version() != 1)
		{
			return Library::Unexpected{ UnwindError::UnsupportedUnwindVersion };
		}

		const auto CodeCount = Foundation::Cast::Auto<Foundation::Size>(
			Header->CodeCount
		);

		const auto CodesOffset = CurrentFunction->UnwindInfoAddress +
			sizeof(UnwindInfoHeader);
		const auto CodesSize = CodeCount * sizeof(UnwindCode);

		if (!RangeValid(CodesOffset, CodesSize, ImageSize_))
		{
			return Library::Unexpected{ UnwindError::InvalidUnwindInfo };
		}

		const auto* Codes = Foundation::Cast::Auto<const UnwindCode*>(
			Base_ + CodesOffset
		);

		Foundation::Size Index{};
		Foundation::Uint32 FunctionOffset{};
		Foundation::Bool InProlog{};

		if (FirstInfo)
		{
			FunctionOffset = Foundation::Cast::Auto<Foundation::Uint32>(
				ControlRva - CurrentFunction->BeginAddress
			);
			InProlog = FunctionOffset <= Header->PrologSize;
		}

		while (Index < CodeCount)
		{
			const auto& Code = Codes[Index];
			const auto SlotCount = OperationSlotCount(Code);

			if (Index + SlotCount > CodeCount)
			{
				return Library::Unexpected{ UnwindError::InvalidUnwindInfo };
			}

			if (FirstInfo && InProlog && Code.CodeOffset > FunctionOffset)
			{
				Index += SlotCount;
				continue;
			}
			const auto Operation = Code.Operation();
			const auto OperationInfo = Code.OperationInfo();

			switch (Operation)
			{
			case UnwindOperation::PushNonvolatile:
			{
				auto RegisterValue = Register(Caller, OperationInfo);
				if (!RegisterValue)
				{
					return Library::Unexpected{ RegisterValue.Error() };
				}

				*RegisterValue.Value() = *Foundation::Cast::Auto<Foundation::UintPtr*>(
					Caller.Rsp
				);
				Caller.Rsp += sizeof(Foundation::UintPtr);
				++Index;
				break;
			}

			case UnwindOperation::AllocateLarge:
				if (OperationInfo == 0)
				{
					if (Index + 1 >= CodeCount)
					{
						return Library::Unexpected{ UnwindError::InvalidUnwindInfo };
					}

					Caller.Rsp += Foundation::Cast::Auto<Foundation::UintPtr>(
						ReadUint16(Codes, Index + 1)
					) * 8;
					Index += 2;
				}
				else if (OperationInfo == 1)
				{
					if (Index + 2 >= CodeCount)
					{
						return Library::Unexpected{ UnwindError::InvalidUnwindInfo };
					}

					Caller.Rsp += ReadUint32(Codes, Index + 1);
					Index += 3;
				}
				else
				{
					return Library::Unexpected{ UnwindError::InvalidUnwindInfo };
				}
				break;

			case UnwindOperation::AllocateSmall:
				Caller.Rsp += Foundation::Cast::Auto<Foundation::UintPtr>(
					OperationInfo
				) * 8 + 8;
				++Index;
				break;

			case UnwindOperation::SetFramePointer:
			{
				auto FrameRegister = Register(Caller, Header->FrameRegister());
				if (!FrameRegister)
				{
					return Library::Unexpected{ FrameRegister.Error() };
				}

				Caller.Rsp = *FrameRegister.Value() -
					Foundation::Cast::Auto<Foundation::UintPtr>(Header->FrameOffset()) * 16;
				++Index;
				break;
			}

			case UnwindOperation::SaveNonvolatile:
			{
				if (Index + 1 >= CodeCount)
				{
					return Library::Unexpected{ UnwindError::InvalidUnwindInfo };
				}

				auto RegisterValue = Register(Caller, OperationInfo);
				if (!RegisterValue)
				{
					return Library::Unexpected{ RegisterValue.Error() };
				}

				const auto Offset = Foundation::Cast::Auto<Foundation::UintPtr>(
					ReadUint16(Codes, Index + 1)
				) * 8;

				*RegisterValue.Value() = *Foundation::Cast::Auto<Foundation::UintPtr*>(
					Caller.Rsp + Offset
				);
				Index += 2;
				break;
			}

			case UnwindOperation::SaveNonvolatileFar:
			{
				if (Index + 2 >= CodeCount)
				{
					return Library::Unexpected{ UnwindError::InvalidUnwindInfo };
				}

				auto RegisterValue = Register(Caller, OperationInfo);
				if (!RegisterValue)
				{
					return Library::Unexpected{ RegisterValue.Error() };
				}

				const auto Offset = Foundation::Cast::Auto<Foundation::UintPtr>(
					ReadUint32(Codes, Index + 1)
				);

				*RegisterValue.Value() = *Foundation::Cast::Auto<Foundation::UintPtr*>(
					Caller.Rsp + Offset
				);
				Index += 3;
				break;
			}

			case UnwindOperation::SaveXmm128:
				if (Index + 1 >= CodeCount)
				{
					return Library::Unexpected{ UnwindError::InvalidUnwindInfo };
				}
				Index += 2;
				break;

			case UnwindOperation::SaveXmm128Far:
				if (Index + 2 >= CodeCount)
				{
					return Library::Unexpected{ UnwindError::InvalidUnwindInfo };
				}
				Index += 3;
				break;

			case UnwindOperation::PushMachineFrame:
			default:
				return Library::Unexpected{ UnwindError::UnsupportedUnwindOperation };
			}
		}

		if ((Header->Flags() & ChainedInfoFlag) == 0)
		{
			break;
		}

		const auto ChainedOffset = CodesOffset +
			AlignedCodeCount(CodeCount) * sizeof(UnwindCode);

		if (!RangeValid(ChainedOffset, sizeof(RuntimeFunction), ImageSize_))
		{
			return Library::Unexpected{ UnwindError::InvalidUnwindInfo };
		}

		CurrentFunction = Foundation::Cast::Auto<const RuntimeFunction*>(
			Base_ + ChainedOffset
		);
		FirstInfo = false;
	}

	auto* ReturnAddressSlot = Foundation::Cast::Auto<Foundation::UintPtr*>(
		Caller.Rsp
	);

	Caller.Rip = *ReturnAddressSlot;
	Caller.Rsp += sizeof(Foundation::UintPtr);

	if (Caller.Rsp <= Current.Rsp)
	{
		return Library::Unexpected{ UnwindError::InvalidStackProgress };
	}

	return Frame{ Caller, ReturnAddressSlot };
}
