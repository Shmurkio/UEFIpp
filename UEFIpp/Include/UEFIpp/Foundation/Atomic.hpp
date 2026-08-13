#pragma once

#include <intrin.h>

#include <UEFIpp/Foundation/Assertions.hpp>
#include <UEFIpp/Foundation/Concepts.hpp>
#include <UEFIpp/Foundation/Utility.hpp>

namespace UEFIpp::Foundation
{
	enum class MemoryOrder : Uint8
	{
		Relaxed,
		Acquire,
		Release,
		AcquireRelease,
		Sequential
	};

	template<Concepts::Integral T>
	requires (sizeof(T) == sizeof(Uint32) || sizeof(T) == sizeof(Uint64))
	class Atomic final
	{
	public:
		constexpr Atomic() noexcept = default;

		explicit constexpr Atomic(T Value) noexcept :
			Value_(Value)
		{
		}

		Atomic(const Atomic&) = delete;
		auto operator=(const Atomic&) -> Atomic& = delete;

		[[nodiscard]] auto Load(MemoryOrder = MemoryOrder::Sequential) const noexcept -> T
		{
			if constexpr (sizeof(T) == sizeof(Uint32))
			{
				return static_cast<T>(_InterlockedCompareExchange(
					const_cast<volatile long*>(Storage32()),
					0,
					0
				));
			}
			else
			{
				return static_cast<T>(_InterlockedCompareExchange64(
					const_cast<volatile long long*>(Storage64()),
					0,
					0
				));
			}
		}

		auto Store(T Desired, MemoryOrder = MemoryOrder::Sequential) noexcept -> Void
		{
			Foundation::Utility::Ignore(Exchange(Desired));
		}

		[[nodiscard]] auto Exchange(
			T Desired,
			MemoryOrder = MemoryOrder::Sequential
		) noexcept -> T
		{
			if constexpr (sizeof(T) == sizeof(Uint32))
			{
				return static_cast<T>(_InterlockedExchange(
					Storage32(),
					static_cast<long>(Desired)
				));
			}
			else
			{
				return static_cast<T>(_InterlockedExchange64(
					Storage64(),
					static_cast<long long>(Desired)
				));
			}
		}

		[[nodiscard]] auto CompareExchange(
			T& Expected,
			T Desired,
			MemoryOrder = MemoryOrder::Sequential,
			MemoryOrder = MemoryOrder::Sequential
		) noexcept -> Bool
		{
			T Observed{};

			if constexpr (sizeof(T) == sizeof(Uint32))
			{
				Observed = static_cast<T>(_InterlockedCompareExchange(
					Storage32(),
					static_cast<long>(Desired),
					static_cast<long>(Expected)
				));
			}
			else
			{
				Observed = static_cast<T>(_InterlockedCompareExchange64(
					Storage64(),
					static_cast<long long>(Desired),
					static_cast<long long>(Expected)
				));
			}

			if (Observed == Expected)
			{
				return true;
			}

			Expected = Observed;
			return false;
		}

		[[nodiscard]] auto FetchAdd(
			T Delta,
			MemoryOrder = MemoryOrder::Sequential
		) noexcept -> T
		{
			if constexpr (sizeof(T) == sizeof(Uint32))
			{
				return static_cast<T>(_InterlockedExchangeAdd(
					Storage32(),
					static_cast<long>(Delta)
				));
			}
			else
			{
				return static_cast<T>(_InterlockedExchangeAdd64(
					Storage64(),
					static_cast<long long>(Delta)
				));
			}
		}

		[[nodiscard]] auto FetchSub(
			T Delta,
			MemoryOrder Order = MemoryOrder::Sequential
		) noexcept -> T
		{
			return FetchAdd(static_cast<T>(T{} - Delta), Order);
		}

	private:
		[[nodiscard]] auto Storage32() noexcept -> volatile long*
		{
			return reinterpret_cast<volatile long*>(&Value_);
		}

		[[nodiscard]] auto Storage32() const noexcept -> const volatile long*
		{
			return reinterpret_cast<const volatile long*>(&Value_);
		}

		[[nodiscard]] auto Storage64() noexcept -> volatile long long*
		{
			return reinterpret_cast<volatile long long*>(&Value_);
		}

		[[nodiscard]] auto Storage64() const noexcept -> const volatile long long*
		{
			return reinterpret_cast<const volatile long long*>(&Value_);
		}

		alignas(sizeof(T)) T Value_{};
	};
}
