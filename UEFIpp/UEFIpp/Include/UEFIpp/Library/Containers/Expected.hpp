#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Optional.hpp>

namespace UEFIpp::Library
{
    struct UnexpectType
    {
        explicit constexpr UnexpectType() = default;
    };

    inline constexpr UnexpectType Unexpect{};

    template<typename E>
    class Unexpected
    {
    public:
        using ErrorType = E;

    public:
        constexpr explicit Unexpected(
            const E& Error
        ) : Error_(Error)
        {
        }

        constexpr explicit Unexpected(
            E&& Error
        ) : Error_(Foundation::Utility::Move(Error))
        {
        }

        [[nodiscard]]
        constexpr auto Error() & -> E&
        {
            return Error_;
        }

        [[nodiscard]]
        constexpr auto Error() const& -> const E&
        {
            return Error_;
        }

        [[nodiscard]]
        constexpr auto Error() && -> E&&
        {
            return Foundation::Utility::Move(Error_);
        }

    private:
        E Error_;
    };

    template<typename E>
    Unexpected(E) -> Unexpected<E>;

    template<typename E>
    [[nodiscard]]
    constexpr auto MakeUnexpected(
        E&& Error
    )
    {
        using ErrorType = Foundation::Traits::RemoveCvReferenceType<E>;
        return Unexpected<ErrorType>{
            Foundation::Utility::Forward<E>(Error)
        };
    }

    template<typename T, typename E>
    class Expected
    {
    public:
        using ValueType = T;
        using ErrorType = E;

    private:
        static constexpr Foundation::Size StorageSize = sizeof(T) > sizeof(E) ? sizeof(T) : sizeof(E);

    public:
        Expected()
        {
            ConstructValue();
        }

        Expected(
            const T& Value
        )
        {
            ConstructValue(Value);
        }

        Expected(
            T&& Value
        )
        {
            ConstructValue(
                Foundation::Utility::Move(Value)
            );
        }

        Expected(
            const Unexpected<E>& Error
        )
        {
            ConstructError(
                Error.Error()
            );
        }

        Expected(
            Unexpected<E>&& Error
        )
        {
            ConstructError(
                Foundation::Utility::Move(Error).Error()
            );
        }

        template<typename... TArgs>
        explicit Expected(
            InPlaceType, TArgs&&... Args
        )
        {
            ConstructValue(
                Foundation::Utility::Forward<TArgs>(Args)...
            );
        }

        template<typename... TArgs>
        explicit Expected(
            UnexpectType,
            TArgs&&... Args
        )
        {
            ConstructError(
                Foundation::Utility::Forward<TArgs>(Args)...
            );
        }

        Expected(
            const Expected& Other
        )
        {
            if (Other.HasValue())
            {
                ConstructValue(
                    Other.Value()
                );
            }
            else
            {
                ConstructError(
                    Other.Error()
                );
            }
        }

        Expected(
            Expected&& Other
        )
        {
            if (Other.HasValue())
            {
                ConstructValue(
                    Foundation::Utility::Move(Other.Value())
                );
            }
            else
            {
                ConstructError(
                    Foundation::Utility::Move(Other.Error())
                );
            }
        }

        ~Expected()
        {
            Destroy();
        }

        auto operator=(const Expected& Other) -> Expected&
        {
            if (this == &Other)
            {
                return *this;
            }

            Destroy();

            if (Other.HasValue())
            {
                ConstructValue(
                    Other.Value()
                );
            }
            else
            {
                ConstructError(
                    Other.Error()
                );
            }

            return *this;
        }

        auto operator=(Expected&& Other) -> Expected&
        {
            if (this == &Other)
            {
                return *this;
            }

            Destroy();

            if (Other.HasValue())
            {
                ConstructValue(
                    Foundation::Utility::Move(Other.Value())
                );
            }
            else
            {
                ConstructError(
                    Foundation::Utility::Move(Other.Error())
                );
            }

            return *this;
        }

        [[nodiscard]]
        auto HasValue() const -> Foundation::Bool
        {
            return HasValue_;
        }

        [[nodiscard]]
        explicit operator Foundation::Bool() const
        {
            return HasValue();
        }

        [[nodiscard]]
        auto Value() & -> T&
        {
            UEFIPP_ASSERT(
                HasValue_
            );

            return *ValuePointer();
        }

        [[nodiscard]]
        auto Value() const& -> const T&
        {
            UEFIPP_ASSERT(
                HasValue_
            );

            return *ValuePointer();
        }

        [[nodiscard]]
        auto Value() && -> T&&
        {
            UEFIPP_ASSERT(
                HasValue_
            );

            return Foundation::Utility::Move(
                *ValuePointer()
            );
        }

        [[nodiscard]]
        auto Error() & -> E&
        {
            UEFIPP_ASSERT(
                !HasValue_
            );

            return *ErrorPointer();
        }

        [[nodiscard]]
        auto Error() const& -> const E&
        {
            UEFIPP_ASSERT(
                !HasValue_
            );

            return *ErrorPointer();
        }

        [[nodiscard]]
        auto Error() && -> E&&
        {
            UEFIPP_ASSERT(
                !HasValue_
            );

            return Foundation::Utility::Move(
                *ErrorPointer()
            );
        }

        [[nodiscard]]
        auto operator*() & -> T&
        {
            return Value();
        }

        [[nodiscard]]
        auto operator*() const& -> const T&
        {
            return Value();
        }

        [[nodiscard]]
        auto operator*() && -> T&&
        {
            return Foundation::Utility::Move(
                *this
            ).Value();
        }

        [[nodiscard]]
        auto operator->() -> T*
        {
            return &Value();
        }

        [[nodiscard]]
        auto operator->() const -> const T*
        {
            return &Value();
        }

        template<typename U>
        [[nodiscard]]
        auto ValueOr(
            U&& Fallback
        ) const& -> T
        {
            if (HasValue_)
            {
                return Value();
            }

            return T(
                Foundation::Utility::Forward<U>(Fallback)
            );
        }

        template<typename U>
        [[nodiscard]] auto ErrorOr(
            U&& Fallback
        ) const& -> E
        {
            if (!HasValue_)
            {
                return Error();
            }

            return E(
                Foundation::Utility::Forward<U>(Fallback)
            );
        }

        template<typename... TArgs>
        auto Emplace(
            TArgs&&... Args
        ) -> T&
        {
            Destroy();
            ConstructValue(
                Foundation::Utility::Forward<TArgs>(Args)...
            );

            return Value();
        }

        template<typename... TArgs>
        auto EmplaceError(
            TArgs&&... Args
        ) -> E&
        {
            Destroy();
            ConstructError(
                Foundation::Utility::Forward<TArgs>(Args)...
            );

            return Error();
        }

        template<typename TFunction>
        [[nodiscard]]
        auto AndThen(
            TFunction Function
        )&
        {
            using ReturnType = decltype(Function(Value()));

            if (HasValue_)
            {
                return Function(
                    Value()
                );
            }

            return ReturnType(
                Unexpect,
                Error()
            );
        }

        template<typename TFunction>
        [[nodiscard]]
        auto AndThen(
            TFunction Function
        ) const&
        {
            using ReturnType = decltype(Function(Value()));

            if (HasValue_)
            {
                return Function(
                    Value()
                );
            }

            return ReturnType(
                Unexpect, Error()
            );
        }

        template<typename TFunction>
        [[nodiscard]]
        auto AndThen(
            TFunction Function
        )&&
        {
            using ReturnType = decltype(Function(
                Foundation::Utility::Move(*this).Value()
            ));

            if (HasValue_)
            {
                return Function(
                    Foundation::Utility::Move(*this).Value()
                );
            }

            return ReturnType(
                Unexpect,
                Foundation::Utility::Move(*this).Error()
            );
        }

        template<typename TFunction>
        [[nodiscard]]
        auto Transform(
            TFunction Function
        ) const&
        {
            using ResultType = decltype(Function(
                Value()
            ));

            if (HasValue_)
            {
                return Expected<ResultType, E>(
                    InPlace,
                    Function(Value())
                );
            }

            return Expected<ResultType, E>(
                Unexpect,
                Error()
            );
        }

        template<typename TFunction>
        [[nodiscard]]
        auto Transform(
            TFunction Function
        )&&
        {
            using ResultType = decltype(Function(
                Foundation::Utility::Move(*this).Value()
            ));

            if (HasValue_)
            {
                return Expected<ResultType, E>(
                    InPlace,
                    Function(Foundation::Utility::Move(*this).Value())
                );
            }

            return Expected<ResultType, E>(
                Unexpect,
                Foundation::Utility::Move(*this).Error()
            );
        }

        template<typename TFunction>
        [[nodiscard]]
        auto TransformError(
            TFunction Function
        ) const&
        {
            using ResultErrorType = decltype(Function(
                Error()
            ));

            if (HasValue_)
            {
                return Expected<T, ResultErrorType>(
                    InPlace,
                    Value()
                );
            }

            return Expected<T, ResultErrorType>(
                Unexpect,
                Function(Error())
            );
        }

        template<typename TFunction>
        [[nodiscard]]
        auto OrElse(
            TFunction Function
        ) const& -> Expected
        {
            if (HasValue_)
            {
                return *this;
            }

            return Function(
                Error()
            );
        }

    private:
        auto Destroy() -> Foundation::Void
        {
            if (HasValue_)
            {
                ValuePointer()->~T();
            }
            else
            {
                ErrorPointer()->~E();
            }
        }

        [[nodiscard]]
        auto ValuePointer() -> T*
        {
            return Foundation::Cast::Pointer<T>(
                Storage_
            );
        }

        [[nodiscard]]
        auto ValuePointer() const -> const T*
        {
            return Foundation::Cast::Pointer<const T>(
                Storage_
            );
        }

        [[nodiscard]]
        auto ErrorPointer() -> E*
        {
            return Foundation::Cast::Pointer<E>(
                Storage_
            );
        }

        [[nodiscard]]
        auto ErrorPointer() const -> const E*
        {
            return Foundation::Cast::Pointer<const E>(
                Storage_
            );
        }

        template<typename... TArgs>
        auto ConstructValue(
            TArgs&&... Args
        ) -> Foundation::Void
        {
            ::new (Foundation::Cast::Pointer<Foundation::Void*>(Storage_))
                T(Foundation::Utility::Forward<TArgs>(Args)...);

            HasValue_ = true;
        }

        template<typename... TArgs>
        auto ConstructError(
            TArgs&&... Args
        ) -> Foundation::Void
        {
            ::new (Foundation::Cast::Pointer<Foundation::Void*>(Storage_))
                E(Foundation::Utility::Forward<TArgs>(Args)...);

            HasValue_ = false;
        }

    private:
        alignas(T) alignas(E) Foundation::Byte Storage_[StorageSize]{};
        Foundation::Bool HasValue_{ true };
    };

    template<typename E>
    class Expected<Foundation::Void, E>
    {
    public:
        using ValueType = Foundation::Void;
        using ErrorType = E;

    public:
        constexpr Expected() = default;

        Expected(
            const Unexpected<E>& Error
        )
        {
            ConstructError(
                Error.Error()
            );
        }

        Expected(
            Unexpected<E>&& Error
        )
        {
            ConstructError(
                Foundation::Utility::Move(Error).Error()
            );
        }

        template<typename... TArgs>
        explicit Expected(
            UnexpectType, TArgs&&... Args
        )
        {
            ConstructError(
                Foundation::Utility::Forward<TArgs>(Args)...
            );
        }

        Expected(
            const Expected& Other
        )
        {
            if (!Other.HasValue())
            {
                ConstructError(
                    Other.Error()
                );
            }
        }

        Expected(
            Expected&& Other
        )
        {
            if (!Other.HasValue())
            {
                ConstructError(
                    Foundation::Utility::Move(Other.Error())
                );
            }
        }

        ~Expected()
        {
            Destroy();
        }

        auto operator=(const Expected& Other) -> Expected&
        {
            if (this == &Other)
            {
                return *this;
            }

            Destroy();

            if (!Other.HasValue())
            {
                ConstructError(
                    Other.Error()
                );
            }

            return *this;
        }

        auto operator=(Expected&& Other) -> Expected&
        {
            if (this == &Other)
            {
                return *this;
            }

            Destroy();

            if (!Other.HasValue())
            {
                ConstructError(
                    Foundation::Utility::Move(Other.Error())
                );
            }

            return *this;
        }

        [[nodiscard]]
        auto HasValue() const -> Foundation::Bool
        {
            return HasValue_;
        }

        [[nodiscard]]
        explicit operator Foundation::Bool() const
        {
            return HasValue();
        }

        auto Value() const -> Foundation::Void
        {
            UEFIPP_ASSERT(
                HasValue_
            );
        }

        [[nodiscard]]
        auto Error() & -> E&
        {
            UEFIPP_ASSERT(
                !HasValue_
            );

            return *ErrorPointer();
        }

        [[nodiscard]]
        auto Error() const& -> const E&
        {
            UEFIPP_ASSERT(
                !HasValue_
            );

            return *ErrorPointer();
        }

        [[nodiscard]]
        auto Error() && -> E&&
        {
            UEFIPP_ASSERT(
                !HasValue_
            );

            return Foundation::Utility::Move(
                *ErrorPointer()
            );
        }

        template<typename U>
        [[nodiscard]]
        auto ErrorOr(
            U&& Fallback
        ) const& -> E
        {
            if (!HasValue_)
            {
                return Error();
            }

            return E(
                Foundation::Utility::Forward<U>(Fallback)
            );
        }

        template<typename... TArgs>
        auto EmplaceError(
            TArgs&&... Args
        ) -> E&
        {
            Destroy();
            ConstructError(
                Foundation::Utility::Forward<TArgs>(Args)...
            );

            return Error();
        }

        template<typename TFunction>
        [[nodiscard]]
        auto AndThen(
            TFunction Function
        ) const&
        {
            using ReturnType = decltype(Function());

            if (HasValue_)
            {
                return Function();
            }

            return ReturnType(
                Unexpect, Error()
            );
        }

        template<typename TFunction>
        [[nodiscard]]
        auto Transform(
            TFunction Function
        ) const&
        {
            using ResultType = decltype(Function());

            if (HasValue_)
            {
                return Expected<ResultType, E>(
                    InPlace, Function()
                );
            }

            return Expected<ResultType, E>(
                Unexpect, Error()
            );
        }

        template<typename TFunction>
        [[nodiscard]]
        auto OrElse(
            TFunction Function
        ) const& -> Expected
        {
            if (HasValue_)
            {
                return *this;
            }

            return Function(
                Error()
            );
        }

    private:
        auto Destroy() -> Foundation::Void
        {
            if (!HasValue_)
            {
                ErrorPointer()->~E();
                HasValue_ = true;
            }
        }

        [[nodiscard]]
        auto ErrorPointer() -> E*
        {
            return Foundation::Cast::Pointer<E*>(
                Storage_
            );
        }

        [[nodiscard]]
        auto ErrorPointer() const -> const E*
        {
            return Foundation::Cast::Pointer<const E*>(
                Storage_
            );
        }

        template<typename... TArgs>
        auto ConstructError(
            TArgs&&... Args
        ) -> Foundation::Void
        {
            ::new (Foundation::Cast::Pointer<Foundation::Void*>(Storage_))
                E(Foundation::Utility::Forward<TArgs>(Args)...);

            HasValue_ = false;
        }

    private:
        alignas(E) Foundation::Byte Storage_[sizeof(E)]{};
        Foundation::Bool HasValue_{ true };
    };
}