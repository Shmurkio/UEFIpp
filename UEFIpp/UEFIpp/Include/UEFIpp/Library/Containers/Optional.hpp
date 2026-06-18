#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::Library
{
	struct NullOptType
	{
		explicit constexpr NullOptType(int)
		{
		}
	};

	inline constexpr NullOptType NullOpt{ 0 };

	struct InPlaceType
	{
		explicit constexpr InPlaceType() = default;
	};

	inline constexpr InPlaceType InPlace{};

	template<typename T>
	class Optional
	{
	public:
		using ValueType = T;

	public:
		constexpr Optional() = default;

		constexpr Optional(NullOptType)
		{
		}

		Optional(const T& Value)
		{
			Construct(Value);
		}

		Optional(T&& Value)
		{
			Construct(static_cast<T&&>(Value));
		}

		template<typename... TArgs>
		explicit Optional(InPlaceType, TArgs&&... Args)
		{
			Construct(Foundation::Cast::Auto<TArgs&&>(Args)...);
		}

		Optional(const Optional& Other)
		{
			if (Other.HasValue())
			{
				Construct(Other.Value());
			}
		}

		Optional(Optional&& Other)
		{
			if (Other.HasValue())
			{
				Construct(static_cast<T&&>(Other.Value()));
				Other.Reset();
			}
		}

		~Optional()
		{
			Reset();
		}

		auto operator=(NullOptType) -> Optional&
		{
			Reset();
			return *this;
		}

		auto operator=(const Optional& Other) -> Optional&
		{
			if (this == &Other)
			{
				return *this;
			}

			if (Other.HasValue())
			{
				Assign(Other.Value());
			}
			else
			{
				Reset();
			}

			return *this;
		}

		auto operator=(Optional&& Other) -> Optional&
		{
			if (this == &Other)
			{
				return *this;
			}

			if (Other.HasValue())
			{
				Assign(static_cast<T&&>(Other.Value()));
				Other.Reset();
			}
			else
			{
				Reset();
			}

			return *this;
		}

		auto operator=(const T& Value) -> Optional&
		{
			Assign(Value);
			return *this;
		}

		auto operator=(T&& Value) -> Optional&
		{
			Assign(Foundation::Cast::Auto<T&&>(Value));
			return *this;
		}

		[[nodiscard]] auto HasValue() const -> Foundation::Bool
		{
			return HasValue_;
		}

		[[nodiscard]] explicit operator Foundation::Bool() const
		{
			return HasValue();
		}

		[[nodiscard]] auto Value() -> T&
		{
			return *Pointer();
		}

		[[nodiscard]] auto Value() const -> const T&
		{
			return *Pointer();
		}

		[[nodiscard]] auto operator*() -> T&
		{
			return Value();
		}

		[[nodiscard]] auto operator*() const -> const T&
		{
			return Value();
		}

		[[nodiscard]] auto operator->() -> T*
		{
			return Pointer();
		}

		[[nodiscard]] auto operator->() const -> const T*
		{
			return Pointer();
		}

		template<typename U>
		[[nodiscard]] auto ValueOr(U&& Fallback) const -> T
		{
			if (HasValue_)
			{
				return Value();
			}

			return T(Foundation::Cast::Auto<U&&>(Fallback));
		}

		template<typename... TArgs>
		auto Emplace(TArgs&&... Args) -> T&
		{
			Reset();
			Construct(Foundation::Cast::Auto<TArgs&&>(Args)...);
			return Value();
		}

		auto Reset() -> void
		{
			if (HasValue_)
			{
				Pointer()->~T();
				HasValue_ = false;
			}
		}

		auto Swap(Optional& Other) -> void
		{
			if (HasValue_ && Other.HasValue_)
			{
				auto Temp = Foundation::Cast::Auto<T&&>(Value());
				Value() = Foundation::Cast::Auto<T&&>(Other.Value());
				Other.Value() = Foundation::Cast::Auto<T&&>(Temp);
			}
			else if (HasValue_)
			{
				Other.Construct(Foundation::Cast::Auto<T&&>(Value()));
				Reset();
			}
			else if (Other.HasValue_)
			{
				Construct(Foundation::Cast::Auto<T&&>(Other.Value()));
				Other.Reset();
			}
		}

		template<typename TFunction>
		auto AndThen(TFunction Function)&
		{
			if (HasValue_)
			{
				return Function(Value());
			}

			using ReturnType = decltype(Function(Value()));
			return ReturnType{};
		}

		template<typename TFunction>
		auto Transform(TFunction Function) const
		{
			using ResultType = decltype(Function(Value()));

			if (HasValue_)
			{
				return Optional<ResultType>(Function(Value()));
			}

			return Optional<ResultType>{};
		}

		template<typename TFunction>
		auto OrElse(TFunction Function) const -> Optional
		{
			if (HasValue_)
			{
				return *this;
			}

			return Function();
		}

		[[nodiscard]] auto operator==(NullOptType) const -> Foundation::Bool
		{
			return !HasValue_;
		}

		[[nodiscard]] auto operator!=(NullOptType) const -> Foundation::Bool
		{
			return HasValue_;
		}

		template<typename U = T>
		[[nodiscard]] auto operator==(const Optional& Other) const -> Foundation::Bool requires requires(const U& A, const U& B)
		{
			A == B;
		}
		{
			if (HasValue_ != Other.HasValue_)
			{
				return false;
			}

			return !HasValue_ || Value() == Other.Value();
		}

		template<typename U = T>
		[[nodiscard]] auto operator==(const T& Other) const -> Foundation::Bool requires requires(const U& A, const T& B)
		{
			A == B;
		}
		{
			return HasValue_ && Value() == Other;
		}

	private:
		[[nodiscard]] auto Pointer() -> T*
		{
			return reinterpret_cast<T*>(Storage_);
		}

		[[nodiscard]] auto Pointer() const -> const T*
		{
			return reinterpret_cast<const T*>(Storage_);
		}

		template<typename... TArgs>
		auto Construct(TArgs&&... Args) -> void
		{
			::new (static_cast<void*>(Storage_)) T(static_cast<TArgs&&>(Args)...);
			HasValue_ = true;
		}

		template<typename U>
		auto Assign(U&& Value) -> void
		{
			if (HasValue_)
			{
				this->Value() = static_cast<U&&>(Value);
			}
			else
			{
				Construct(static_cast<U&&>(Value));
			}
		}

	private:
		alignas(T) unsigned char Storage_[sizeof(T)]{};
		Foundation::Bool HasValue_{};
	};
}