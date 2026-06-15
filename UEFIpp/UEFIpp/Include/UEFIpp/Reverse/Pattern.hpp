#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Optional.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>
#include <UEFIpp/Library/String/StringView.hpp>
#include <UEFIpp/Reverse/PatternByte.hpp>

namespace UEFIpp::Reverse
{
	class Pattern
	{
	public:
		using ByteType = PatternByte;
		using StorageType = Library::Vector<ByteType>;
		using SpanType = Library::Span<const ByteType>;
		using StringViewType = Library::StringView;

	public:
		Pattern() = default;

		explicit Pattern(StorageType Bytes) : Bytes_(Foundation::Utility::Move(Bytes))
		{
		}

		~Pattern() = default;

		Pattern(const Pattern&) = default;
		Pattern(Pattern&&) noexcept = default;

		auto operator=(const Pattern&) -> Pattern & = default;
		auto operator=(Pattern&&) noexcept -> Pattern & = default;

		auto operator==(const Pattern&) const->Foundation::Bool = default;

	public:
		[[nodiscard]] static auto Compile(StringViewType Text)
			-> Library::Optional<Pattern>;

		[[nodiscard]] auto Bytes() const -> SpanType
		{
			return SpanType(Bytes_.Data(), Bytes_.Size());
		}

		[[nodiscard]] auto Size() const -> Foundation::Size
		{
			return Bytes_.Size();
		}

		[[nodiscard]] auto Empty() const -> Foundation::Bool
		{
			return Bytes_.Empty();
		}

		[[nodiscard]] auto Front() const -> const ByteType&
		{
			return Bytes_.Front();
		}

		[[nodiscard]] auto Back() const -> const ByteType&
		{
			return Bytes_.Back();
		}

	private:
		StorageType Bytes_{};
	};
}