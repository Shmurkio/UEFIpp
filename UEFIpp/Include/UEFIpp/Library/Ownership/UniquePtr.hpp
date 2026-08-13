#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::Library
{
	template<typename T>
	class DefaultDelete
	{
	public:
		constexpr DefaultDelete() noexcept = default;

		constexpr auto operator()(T* Pointer) const noexcept -> Foundation::Void
		{
			static_assert(!Foundation::Traits::IsVoid<T>::Value);
			delete Pointer;
		}
	};

	template<typename T>
	class DefaultDelete<T[]>
	{
	public:
		constexpr DefaultDelete() noexcept = default;

		template<typename U>
		constexpr auto operator()(U* Pointer) const noexcept -> Foundation::Void
		{
			delete[] Pointer;
		}
	};

	template<typename T, typename TDeleter = DefaultDelete<T>>
	class UniquePtr
	{
	public:
		using ElementType = T;
		using DeleterType = TDeleter;
		using Pointer = T*;

		constexpr UniquePtr() noexcept = default;

		constexpr UniquePtr(Foundation::NullPtr) noexcept
		{
		}

		explicit constexpr UniquePtr(Pointer PointerValue) noexcept :
			Pointer_(PointerValue)
		{
		}

		constexpr UniquePtr(
			Pointer PointerValue,
			const DeleterType& Deleter
		) noexcept :
			Pointer_(PointerValue),
			Deleter_(Deleter)
		{
		}

		constexpr UniquePtr(
			Pointer PointerValue,
			DeleterType&& Deleter
		) noexcept :
			Pointer_(PointerValue),
			Deleter_(Foundation::Utility::Move(Deleter))
		{
		}

		UniquePtr(const UniquePtr&) = delete;
		auto operator=(const UniquePtr&) -> UniquePtr& = delete;

		constexpr UniquePtr(UniquePtr&& Other) noexcept :
			Pointer_(Other.Release()),
			Deleter_(Foundation::Utility::Move(Other.Deleter_))
		{
		}

		constexpr auto operator=(UniquePtr&& Other) noexcept -> UniquePtr&
		{
			if (this == &Other)
			{
				return *this;
			}

			Reset();
			Deleter_ = Foundation::Utility::Move(Other.Deleter_);
			Pointer_ = Other.Release();
			return *this;
		}

		constexpr ~UniquePtr() noexcept
		{
			Reset();
		}

		constexpr auto operator=(Foundation::NullPtr) noexcept -> UniquePtr&
		{
			Reset();
			return *this;
		}

		[[nodiscard]] constexpr auto Get() const noexcept -> Pointer
		{
			return Pointer_;
		}

		[[nodiscard]] constexpr auto GetDeleter() noexcept -> DeleterType&
		{
			return Deleter_;
		}

		[[nodiscard]] constexpr auto GetDeleter() const noexcept -> const DeleterType&
		{
			return Deleter_;
		}

		[[nodiscard]] constexpr explicit operator Foundation::Bool() const noexcept
		{
			return Pointer_ != nullptr;
		}

		[[nodiscard]] constexpr auto operator*() const noexcept -> T&
		{
			UEFIPP_ASSERT(Pointer_ != nullptr);
			return *Pointer_;
		}

		[[nodiscard]] constexpr auto operator->() const noexcept -> Pointer
		{
			UEFIPP_ASSERT(Pointer_ != nullptr);
			return Pointer_;
		}

		[[nodiscard]] constexpr auto Release() noexcept -> Pointer
		{
			return Foundation::Utility::Exchange(Pointer_, nullptr);
		}

		constexpr auto Reset(Pointer NewPointer = nullptr) noexcept -> Foundation::Void
		{
			if (Pointer_ == NewPointer)
			{
				return;
			}

			auto* OldPointer = Foundation::Utility::Exchange(Pointer_, NewPointer);

			if (OldPointer)
			{
				(void)Deleter_(OldPointer);
			}
		}

		constexpr auto Swap(UniquePtr& Other) noexcept -> Foundation::Void
		{
			Foundation::Utility::Swap(Pointer_, Other.Pointer_);
			Foundation::Utility::Swap(Deleter_, Other.Deleter_);
		}

	private:
		Pointer Pointer_{};
		[[no_unique_address]] DeleterType Deleter_{};
	};

	template<typename T, typename TDeleter>
	class UniquePtr<T[], TDeleter>
	{
	public:
		using ElementType = T;
		using DeleterType = TDeleter;
		using Pointer = T*;

		constexpr UniquePtr() noexcept = default;

		constexpr UniquePtr(Foundation::NullPtr) noexcept
		{
		}

		explicit constexpr UniquePtr(Pointer PointerValue) noexcept :
			Pointer_(PointerValue)
		{
		}

		constexpr UniquePtr(
			Pointer PointerValue,
			const DeleterType& Deleter
		) noexcept :
			Pointer_(PointerValue),
			Deleter_(Deleter)
		{
		}

		constexpr UniquePtr(
			Pointer PointerValue,
			DeleterType&& Deleter
		) noexcept :
			Pointer_(PointerValue),
			Deleter_(Foundation::Utility::Move(Deleter))
		{
		}

		UniquePtr(const UniquePtr&) = delete;
		auto operator=(const UniquePtr&) -> UniquePtr& = delete;

		constexpr UniquePtr(UniquePtr&& Other) noexcept :
			Pointer_(Other.Release()),
			Deleter_(Foundation::Utility::Move(Other.Deleter_))
		{
		}

		constexpr auto operator=(UniquePtr&& Other) noexcept -> UniquePtr&
		{
			if (this == &Other)
			{
				return *this;
			}

			Reset();
			Deleter_ = Foundation::Utility::Move(Other.Deleter_);
			Pointer_ = Other.Release();
			return *this;
		}

		constexpr ~UniquePtr() noexcept
		{
			Reset();
		}

		constexpr auto operator=(Foundation::NullPtr) noexcept -> UniquePtr&
		{
			Reset();
			return *this;
		}

		[[nodiscard]] constexpr auto Get() const noexcept -> Pointer
		{
			return Pointer_;
		}

		[[nodiscard]] constexpr auto GetDeleter() noexcept -> DeleterType&
		{
			return Deleter_;
		}

		[[nodiscard]] constexpr auto GetDeleter() const noexcept -> const DeleterType&
		{
			return Deleter_;
		}

		[[nodiscard]] constexpr explicit operator Foundation::Bool() const noexcept
		{
			return Pointer_ != nullptr;
		}

		[[nodiscard]] constexpr auto operator[](Foundation::Size Index) const noexcept -> T&
		{
			UEFIPP_ASSERT(Pointer_ != nullptr);
			return Pointer_[Index];
		}

		[[nodiscard]] constexpr auto Release() noexcept -> Pointer
		{
			return Foundation::Utility::Exchange(Pointer_, nullptr);
		}

		constexpr auto Reset(Pointer NewPointer = nullptr) noexcept -> Foundation::Void
		{
			if (Pointer_ == NewPointer)
			{
				return;
			}

			auto* OldPointer = Foundation::Utility::Exchange(Pointer_, NewPointer);

			if (OldPointer)
			{
				(void)Deleter_(OldPointer);
			}
		}

		constexpr auto Swap(UniquePtr& Other) noexcept -> Foundation::Void
		{
			Foundation::Utility::Swap(Pointer_, Other.Pointer_);
			Foundation::Utility::Swap(Deleter_, Other.Deleter_);
		}

	private:
		Pointer Pointer_{};
		[[no_unique_address]] DeleterType Deleter_{};
	};

	template<typename T, typename TDeleter>
	[[nodiscard]] constexpr auto operator==(
		const UniquePtr<T, TDeleter>& Pointer,
		Foundation::NullPtr
	) noexcept -> Foundation::Bool
	{
		return !Pointer;
	}

	template<typename T, typename TDeleter>
	[[nodiscard]] constexpr auto operator==(
		Foundation::NullPtr,
		const UniquePtr<T, TDeleter>& Pointer
	) noexcept -> Foundation::Bool
	{
		return !Pointer;
	}

	template<typename T, typename TDeleter>
	constexpr auto Swap(
		UniquePtr<T, TDeleter>& Left,
		UniquePtr<T, TDeleter>& Right
	) noexcept -> Foundation::Void
	{
		Left.Swap(Right);
	}
}
