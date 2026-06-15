#include <UEFIpp/Memory/Memory.hpp>

namespace UEFIpp::Memory
{
	auto Copy(Foundation::Void* Destination, const Foundation::Void* Source, Foundation::Size NumberOfBytes) -> Foundation::Void*
	{
		if (!Destination || !Source || !NumberOfBytes)
		{
			return Destination;
		}

		auto* DestinationBytes = Foundation::Cast::Auto<Foundation::Uint8*>(Destination);
		const auto* SourceBytes = Foundation::Cast::Auto<const Foundation::Uint8*>(Source);

		for (Foundation::Size i = 0; i < NumberOfBytes; ++i)
		{
			DestinationBytes[i] = SourceBytes[i];
		}

		return Destination;
	}

	auto Move(Foundation::Void* Destination, const Foundation::Void* Source, Foundation::Size NumberOfBytes) -> Foundation::Void*
	{
		if (!Destination || !Source || !NumberOfBytes)
		{
			return Destination;
		}

		auto* DestinationBytes = Foundation::Cast::Auto<Foundation::Uint8*>(Destination);
		const auto* SourceBytes = Foundation::Cast::Auto<const Foundation::Uint8*>(Source);

		if (DestinationBytes == SourceBytes)
		{
			return Destination;
		}

		if (DestinationBytes < SourceBytes)
		{
			for (Foundation::Size i = 0; i < NumberOfBytes; ++i)
			{
				DestinationBytes[i] = SourceBytes[i];
			}
		}
		else
		{
			for (Foundation::Size i = NumberOfBytes; i > 0; --i)
			{
				DestinationBytes[i - 1] = SourceBytes[i - 1];
			}
		}

		return Destination;
	}

	auto Set(Foundation::Void* Destination, Foundation::Uint8 Value, Foundation::Size NumberOfBytes) -> Foundation::Void*
	{
		if (!Destination || !NumberOfBytes)
		{
			return Destination;
		}

		auto* DestinationBytes = Foundation::Cast::Auto<Foundation::Uint8*>(Destination);

		for (Foundation::Size i = 0; i < NumberOfBytes; ++i)
		{
			DestinationBytes[i] = Value;
		}

		return Destination;
	}

	auto Zero(Foundation::Void* Destination, Foundation::Size NumberOfBytes) -> Foundation::Void*
	{
		return Set(Destination, 0, NumberOfBytes);
	}

	auto Compare(const Foundation::Void* Left, const Foundation::Void* Right, Foundation::Size NumberOfBytes) -> Foundation::Int32
	{
		if ((!Left || !Right) && NumberOfBytes)
		{
			if (Left == Right)
			{
				return 0;
			}

			return Left ? 1 : -1;
		}

		const auto* LeftBytes = Foundation::Cast::Auto<const Foundation::Uint8*>(Left);
		const auto* RightBytes = Foundation::Cast::Auto<const Foundation::Uint8*>(Right);

		for (Foundation::Size i = 0; i < NumberOfBytes; ++i)
		{
			if (LeftBytes[i] != RightBytes[i])
			{
				return Foundation::Cast::Auto<Foundation::Int32>(LeftBytes[i]) - Foundation::Cast::Auto<Foundation::Int32>(RightBytes[i]);
			}
		}

		return 0;
	}
}