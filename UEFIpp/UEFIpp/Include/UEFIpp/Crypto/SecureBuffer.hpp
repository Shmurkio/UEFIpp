#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>
#include <UEFIpp/Crypto/SecureZero.hpp>

namespace UEFIpp::Crypto
{
	class SecureBuffer
	{
	public:
		using Byte = Foundation::Uint8;
		using SizeType = Foundation::Size;
		using StorageType = Library::Vector<Byte>;
		using SpanType = Library::Span<Byte>;
		using ConstSpanType = Library::Span<const Byte>;

	public:
		constexpr SecureBuffer() = default;

		explicit SecureBuffer(SizeType Size)
		{
			Resize(Size);
		}

		SecureBuffer(const SecureBuffer&) = delete;

		auto operator=(const SecureBuffer&) -> SecureBuffer & = delete;

		SecureBuffer(SecureBuffer&&) noexcept = default;

		auto operator=(SecureBuffer&& Other) noexcept -> SecureBuffer&
		{
			if (this != &Other)
			{
				Clear();
				Data_ = Foundation::Utility::Move(Other.Data_);
			}

			return *this;
		}

		~SecureBuffer()
		{
			Clear();
		}

		auto Resize(SizeType Size) -> Foundation::Bool
		{
			return Data_.Resize(Size);
		}

		auto Reset(SizeType Size) -> Foundation::Bool
		{
			Clear();
			return Data_.Resize(Size);
		}

		auto Clear() -> Foundation::Void
		{
			if (Data_.Empty())
			{
				return;
			}

			SecureZero::Bytes(Span());
			Data_.Clear();
		}

		[[nodiscard]] auto Data() -> Byte*
		{
			return Data_.Data();
		}

		[[nodiscard]] auto Data() const -> const Byte*
		{
			return Data_.Data();
		}

		[[nodiscard]] auto Size() const -> SizeType
		{
			return Data_.Size();
		}

		[[nodiscard]] auto Empty() const -> Foundation::Bool
		{
			return Data_.Empty();
		}

		[[nodiscard]] auto Span() -> SpanType
		{
			return SpanType{ Data_.Data(), Data_.Size() };
		}

		[[nodiscard]] auto Span() const -> ConstSpanType
		{
			return ConstSpanType{ Data_.Data(), Data_.Size() };
		}

		[[nodiscard]] auto Bytes() -> SpanType
		{
			return Span();
		}

		[[nodiscard]] auto Bytes() const -> ConstSpanType
		{
			return Span();
		}

		[[nodiscard]] auto operator[](SizeType Index) -> Byte&
		{
			return Data_[Index];
		}

		[[nodiscard]] auto operator[](SizeType Index) const -> const Byte&
		{
			return Data_[Index];
		}

		[[nodiscard]] auto begin()
		{
			return Data_.begin();
		}

		[[nodiscard]] auto begin() const
		{
			return Data_.begin();
		}

		[[nodiscard]] auto end()
		{
			return Data_.end();
		}

		[[nodiscard]] auto end() const
		{
			return Data_.end();
		}

		[[nodiscard]] auto Front() -> Byte&
		{
			return Data_.Front();
		}

		[[nodiscard]] auto Front() const -> const Byte&
		{
			return Data_.Front();
		}

		[[nodiscard]] auto Back() -> Byte&
		{
			return Data_.Back();
		}

		[[nodiscard]] auto Back() const -> const Byte&
		{
			return Data_.Back();
		}

	private:
		StorageType Data_{};
	};
}