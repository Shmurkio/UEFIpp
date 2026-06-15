#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>
#include <UEFIpp/Library/String/String.hpp>
#include <UEFIpp/Library/String/StringView.hpp>
#include <UEFIpp/Library/Containers/Optional.hpp>
#include <UEFIpp/Memory/Memory.hpp>

namespace UEFIpp::Nvram
{
	class View
	{
	public:
		using Byte = Foundation::Uint8;
		using SizeType = Foundation::Size;
		using ByteSpan = Library::Span<const Byte>;

	public:
		constexpr View() = default;

		constexpr explicit View(ByteSpan Data) : Data_(Data)
		{
		}

		[[nodiscard]] constexpr auto Data() const -> ByteSpan
		{
			return Data_;
		}

		[[nodiscard]] constexpr auto Offset() const -> SizeType
		{
			return Offset_;
		}

		[[nodiscard]] constexpr auto Size() const -> SizeType
		{
			return Data_.Size();
		}

		[[nodiscard]] constexpr auto RemainingSize() const -> SizeType
		{
			return Offset_ <= Data_.Size() ? Data_.Size() - Offset_ : 0;
		}

		[[nodiscard]] constexpr auto Empty() const -> Foundation::Bool
		{
			return RemainingSize() == 0;
		}

		[[nodiscard]] constexpr auto CanRead(SizeType Size) const -> Foundation::Bool
		{
			return Size <= RemainingSize();
		}

		constexpr auto Reset() -> Foundation::Void
		{
			Offset_ = 0;
		}

		constexpr auto Seek(SizeType Offset) -> Foundation::Bool
		{
			if (Offset > Data_.Size())
			{
				return false;
			}

			Offset_ = Offset;

			return true;
		}

		constexpr auto Skip(SizeType Size) -> Foundation::Bool
		{
			if (!CanRead(Size))
			{
				return false;
			}

			Offset_ += Size;

			return true;
		}

		[[nodiscard]] constexpr auto Remaining() const -> ByteSpan
		{
			return Data_.Subspan(Offset_, RemainingSize());
		}

		[[nodiscard]] auto PeekBytes(SizeType Size) const -> Library::Optional<ByteSpan>
		{
			if (!CanRead(Size))
			{
				return Library::NullOpt;
			}

			return Data_.Subspan(Offset_, Size);
		}

		[[nodiscard]] auto ReadBytes(SizeType Size) -> Library::Optional<ByteSpan>
		{
			auto Bytes = PeekBytes(Size);

			if (!Bytes)
			{
				return Library::NullOpt;
			}

			Offset_ += Size;

			return Bytes;
		}

		template<typename T>
		[[nodiscard]] auto Peek() const -> Library::Optional<T>
		{
			static_assert(Foundation::Concepts::TriviallyCopyable<T>);

			if (!CanRead(sizeof(T)))
			{
				return Library::NullOpt;
			}

			T Result{};
			Memory::Copy(&Result, Data_.Data() + Offset_, sizeof(T));

			return Result;
		}

		template<typename T>
		[[nodiscard]] auto Read() -> Library::Optional<T>
		{
			auto Result = Peek<T>();

			if (!Result)
			{
				return Library::NullOpt;
			}

			Offset_ += sizeof(T);

			return Result;
		}

		[[nodiscard]] auto ReadUtf16String() -> Library::Optional<Library::WideString>
		{
			const auto Start = Offset_;

			while (CanRead(sizeof(Foundation::WChar)))
			{
				const auto Character = Read<Foundation::WChar>();

				if (!Character)
				{
					Offset_ = Start;
					return Library::NullOpt;
				}

				if (*Character == 0)
				{
					const auto CharacterCount = ((Offset_ - Start) / sizeof(Foundation::WChar)) - 1;
					const auto* Text = Foundation::Cast::Auto<const Foundation::WChar*>(Data_.Data() + Start);
					return Library::WideString{ Library::WideStringView{ Text, CharacterCount } };
				}
			}

			Offset_ = Start;

			return Library::NullOpt;
		}

	private:
		ByteSpan Data_{};
		SizeType Offset_{};
	};
}