#pragma once

#include <UEFIpp/Foundation/Concepts.hpp>

namespace UEFIpp::Foundation
{
	class Cast
	{
	public:
		Cast() = delete;

		// Performs a normal language-level conversion.
		template<typename T, typename U>
		[[nodiscard]] static constexpr auto To(U Value) -> T
		{
			return static_cast<T>(Value);
		}

		// Converts a pointer to its numeric address representation.
		template<Concepts::Integral T, Concepts::Pointer U>
		[[nodiscard]] static constexpr auto PointerToAddress(U Pointer) -> T
		{
			return reinterpret_cast<T>(Pointer);
		}

		// Converts a numeric address to a pointer.
		template<Concepts::Pointer T, Concepts::Integral U>
		[[nodiscard]] static constexpr auto AddressToPointer(U Address) -> T
		{
			return reinterpret_cast<T>(Address);
		}

		// Reinterprets a mutable pointer as another mutable pointer type.
		template<typename T, typename U>
		[[nodiscard]] static constexpr auto Pointer(U* Value) -> T*
		{
			return reinterpret_cast<T*>(Value);
		}

		// Reinterprets a const pointer as another const pointer type.
		template<typename T, typename U>
		[[nodiscard]] static constexpr auto Pointer(const U* Value) -> const T*
		{
			return reinterpret_cast<const T*>(Value);
		}

		// Reinterprets one reference type as another reference type.
		template<typename T, typename U>
		[[nodiscard]] static constexpr auto Reference(U& Value) -> T&
		{
			return reinterpret_cast<T&>(Value);
		}

		// Returns the integral value stored by an enum.
		template<Concepts::Enum T>
		[[nodiscard]] static constexpr auto Underlying(T Value)
		{
			return static_cast<__underlying_type(T)>(Value);
		}

		// Removes const qualification from a pointer.
		template<typename T>
		[[nodiscard]] static constexpr auto RemoveConst(const T* Value) -> T*
		{
			return const_cast<T*>(Value);
		}

		// Copies raw bytes between same-size trivially-copyable types.
		template<typename T, typename U>
		[[nodiscard]] static constexpr auto Bitwise(const U& Value) -> T
		{
			static_assert(sizeof(T) == sizeof(U));
			static_assert(Concepts::TriviallyCopyable<T>);
			static_assert(Concepts::TriviallyCopyable<U>);

			T Result{};

			const auto Source = reinterpret_cast<const Byte*>(&Value);
			auto Destination = reinterpret_cast<Byte*>(&Result);

			for (Size i = 0; i < sizeof(T); ++i)
			{
				Destination[i] = Source[i];
			}

			return Result;
		}

		// Performs an unrestricted conversion. This helper is intended for low-level
		// systems code where the caller explicitly wants to bypass normal type-safety.
		// It may remove const qualification, reinterpret pointer types, or convert
		// between pointers and numeric addresses.
		template<typename T, typename U>
		[[nodiscard]] static constexpr auto Force(U&& Value) -> T
		{
			using Source = Traits::RemoveReferenceType<U>;
			using SourceNoCv = Traits::RemoveCvType<Source>;

			// Integer address -> pointer
			if constexpr (Concepts::Pointer<T> && Concepts::Integral<SourceNoCv>)
			{
				return AddressToPointer<T>(Value);
			}
			// Pointer -> integer address
			else if constexpr (Concepts::Integral<T> && Concepts::Pointer<SourceNoCv>)
			{
				return PointerToAddress<T>(Value);
			}
			// Pointer -> pointer, allowing const removal and reinterpretation
			else if constexpr (Concepts::Pointer<T> && Concepts::Pointer<SourceNoCv>)
			{
				using TargetPointee = Traits::RemovePointerType<T>;
				using MutableTargetPointee = Traits::RemoveCvType<TargetPointee>;

				return reinterpret_cast<T>(const_cast<MutableTargetPointee*>(reinterpret_cast<const MutableTargetPointee*>(Value)));
			}
			// Normal language-level conversion
			else
			{
				return To<T>(Value);
			}
		}

		// Selects the safest matching cast helper for common low-level cast cases.
		// Pointer/address conversions are handled automatically. Pointer-to-pointer
		// conversions preserve const-correctness whenever possible. If no safe cast
		// exists, Force() is used as a fallback.
		template<typename T, typename U>
		[[nodiscard]] static constexpr auto Auto(U&& Value) -> T
		{
			using Source = Traits::RemoveReferenceType<U>;
			using SourceNoCv = Traits::RemoveCvType<Source>;

			// Integer address -> pointer
			if constexpr (Concepts::Pointer<T> && Concepts::Integral<SourceNoCv>)
			{
				return AddressToPointer<T>(Value);
			}
			// Pointer -> integer address
			else if constexpr (Concepts::Integral<T> && Concepts::Pointer<SourceNoCv>)
			{
				return PointerToAddress<T>(Value);
			}
			// Pointer -> pointer
			else if constexpr (Concepts::Pointer<T> && Concepts::Pointer<SourceNoCv>)
			{
				// Prefer a normal reinterpret_cast when it is valid. This preserves
				// const-correctness and rejects unsafe conversions.
				if constexpr (requires { reinterpret_cast<T>(Value); })
				{
					return reinterpret_cast<T>(Value);
				}
				// Fall back to Force() when the conversion would otherwise be rejected.
				else
				{
					return Force<T>(Value);
				}
			}
			// Normal value conversion
			else if constexpr (requires { static_cast<T>(Value); })
			{
				return To<T>(Value);
			}
			// Last-resort conversion path
			else
			{
				return Force<T>(Value);
			}
		}
	};
}