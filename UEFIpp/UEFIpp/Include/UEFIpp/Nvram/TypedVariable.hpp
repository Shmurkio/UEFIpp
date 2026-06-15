#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Optional.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>
#include <UEFIpp/Memory/Memory.hpp>
#include <UEFIpp/Nvram/Variable.hpp>

namespace UEFIpp::Nvram
{
	class TypedVariable
	{
	public:
		using Byte = Foundation::Uint8;
		using ByteSpan = Library::Span<const Byte>;

	public:
		TypedVariable() = default;

		explicit TypedVariable(const Variable& Variable) : Variable_(&Variable)
		{
		}

		[[nodiscard]] auto Valid() const -> Foundation::Bool
		{
			return Variable_ != nullptr;
		}

		[[nodiscard]] explicit operator Foundation::Bool() const
		{
			return Valid();
		}

		[[nodiscard]] auto Raw() const -> const Variable&
		{
			return *Variable_;
		}

		[[nodiscard]] auto Id() const -> const VariableId&
		{
			return Raw().Id();
		}

		[[nodiscard]] auto Attributes() const -> AttributeMask
		{
			return Raw().Attributes();
		}

		[[nodiscard]] auto Data() const -> const Variable::DataType&
		{
			return Raw().Data();
		}

		[[nodiscard]] auto Size() const -> Foundation::Size
		{
			return Raw().Size();
		}

		[[nodiscard]] auto Empty() const -> Foundation::Bool
		{
			return Raw().Empty();
		}

		[[nodiscard]] auto Bytes() const -> ByteSpan
		{
			return ByteSpan{ Data().Data(), Data().Size() };
		}

		template<typename T>
		[[nodiscard]] auto As() const -> Library::Optional<T>
		{
			if (!Valid() || Size() != sizeof(T))
			{
				return Library::NullOpt;
			}

			T Result{};
			Memory::Copy(&Result, Data().Data(), sizeof(T));

			return Result;
		}

		template<typename T>
		[[nodiscard]] auto AsArray() const -> Library::Optional<Library::Span<const T>>
		{
			if (!Valid() || Size() % sizeof(T) != 0)
			{
				return Library::NullOpt;
			}

			const auto Count = Size() / sizeof(T);
			const auto* Pointer = Foundation::Cast::Auto<const T*>(Data().Data());

			return Library::Span<const T>{ Pointer, Count };
		}

		[[nodiscard]] auto AsBool() const -> Library::Optional<Foundation::Bool>
		{
			const auto Value = As<Foundation::Uint8>();

			if (!Value)
			{
				return Library::NullOpt;
			}

			return *Value != 0;
		}

		[[nodiscard]] auto AsUint8() const -> Library::Optional<Foundation::Uint8>
		{
			return As<Foundation::Uint8>();
		}

		[[nodiscard]] auto AsUint16() const -> Library::Optional<Foundation::Uint16>
		{
			return As<Foundation::Uint16>();
		}

		[[nodiscard]] auto AsUint32() const -> Library::Optional<Foundation::Uint32>
		{
			return As<Foundation::Uint32>();
		}

		[[nodiscard]] auto AsUint64() const -> Library::Optional<Foundation::Uint64>
		{
			return As<Foundation::Uint64>();
		}

		[[nodiscard]] auto AsUint16Array() const -> Library::Optional<Library::Span<const Foundation::Uint16>>
		{
			return AsArray<Foundation::Uint16>();
		}

	private:
		const Variable* Variable_{};
	};
}