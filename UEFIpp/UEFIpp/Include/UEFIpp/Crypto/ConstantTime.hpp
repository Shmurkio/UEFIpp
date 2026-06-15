#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>

namespace UEFIpp::Crypto
{
	class ConstantTime
	{
	public:
		ConstantTime() = delete;

		static auto Equals(Library::Span<const Foundation::Uint8> Left, Library::Span<const Foundation::Uint8> Right) -> Foundation::Bool
		{
			if (Left.Size() != Right.Size())
			{
				return false;
			}

			Foundation::Uint8 Difference{};

			for (Foundation::Size Index{}; Index < Left.Size(); ++Index)
			{
				Difference |= Left[Index] ^ Right[Index];
			}

			return Difference == 0;
		}

		static auto IsZero(Library::Span<const Foundation::Uint8> Data) -> Foundation::Bool
		{
			Foundation::Uint8 Difference{};

			for (Foundation::Size Index{}; Index < Data.Size(); ++Index)
			{
				Difference |= Data[Index];
			}

			return Difference == 0;
		}
	};
}