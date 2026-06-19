#pragma once

#include <UEFIpp/Library/Functional/FunctionDetail.hpp>

namespace UEFIpp::Library
{
	template<typename>
	class Function;

	template<typename Return, typename... Args>
	class Function<Return(Args...)>
	{
	private:
		static constexpr Foundation::Size InlineSize = sizeof(Foundation::Void*) * 4;
		static constexpr Foundation::Size InlineAlign = alignof(Foundation::Void*);

		using InvokeFn = Return(*)(Foundation::Void*, Args...);
		using CopyFn = Foundation::Void(*)(Foundation::Void*, const Foundation::Void*);
		using MoveFn = Foundation::Void(*)(Foundation::Void*, Foundation::Void*);
		using DestroyFn = Foundation::Void(*)(Foundation::Void*);

		struct VTable
		{
			InvokeFn Invoke;
			CopyFn Copy;
			MoveFn Move;
			DestroyFn Destroy;
			Foundation::Size SizeValue;
			Foundation::Size AlignValue;
		};

		template<typename Callable>
		static constexpr Foundation::Bool FitsInline = sizeof(Callable) <= InlineSize && alignof(Callable) <= InlineAlign;

	private:
		alignas(InlineAlign) Foundation::Byte Storage_[InlineSize]{};
		Foundation::Void* Object_{};
		const VTable* Table_{};
		Foundation::Bool Inline_{};

	private:
		template<typename Callable>
		struct TableStorage
		{
			inline static constexpr VTable Instance
			{
				[](Foundation::Void* Object, Args... Arguments) -> Return
				{
					return Detail::Invoke<Return, Callable, Args...>(Object, Foundation::Utility::Forward<Args>(Arguments)...);
				},

				[](Foundation::Void* Destination, const Foundation::Void* Source) -> Foundation::Void
				{
					new (Destination) Callable(*static_cast<const Callable*>(Source));
				},

				[](Foundation::Void* Destination, Foundation::Void* Source) -> Foundation::Void
				{
					new (Destination) Callable(Foundation::Utility::Move(*static_cast<Callable*>(Source)));
				},

				[](Foundation::Void* Object) -> Foundation::Void
				{
					static_cast<Callable*>(Object)->~Callable();
				},

				sizeof(Callable),
				alignof(Callable)
			};
		};

		template<typename Callable>
		static constexpr const VTable* Table()
		{
			return &TableStorage<Callable>::Instance;
		}

		Foundation::Void ResetInternal()
		{
			if (!Table_)
			{
				return;
			}

			Table_->Destroy(Object_);

			if (!Inline_)
			{
				::operator delete(Object_);
			}

			Object_ = nullptr;
			Table_ = nullptr;
			Inline_ = false;
		}

		template<typename Callable>
		Foundation::Void Construct(Callable&& CallableObject)
		{
			using Stored = Foundation::Traits::RemoveCvReferenceType<Callable>;

			Table_ = Table<Stored>();

			if constexpr (FitsInline<Stored>)
			{
				Object_ = Storage_;
				Inline_ = true;
				new (Object_) Stored(Foundation::Utility::Forward<Callable>(CallableObject));
			}
			else
			{
				Object_ = ::operator new(sizeof(Stored));
				Inline_ = false;
				new (Object_) Stored(Foundation::Utility::Forward<Callable>(CallableObject));
			}
		}

	public:
		constexpr Function() = default;

		constexpr Function(Foundation::NullPtr)
		{
		}

		~Function()
		{
			ResetInternal();
		}

		Function(const Function& Other)
		{
			if (!Other.Table_)
				return;

			Table_ = Other.Table_;
			Inline_ = Other.Inline_;

			if (Other.Inline_)
			{
				Object_ = Storage_;
				Table_->Copy(Object_, Other.Object_);
			}
			else
			{
				Object_ = ::operator new(Table_->SizeValue);
				Table_->Copy(Object_, Other.Object_);
			}
		}

		auto operator=(const Function& Other) -> Function&
		{
			if (this == &Other)
			{
				return *this;
			}

			ResetInternal();

			if (!Other.Table_)
			{
				return *this;
			}

			Table_ = Other.Table_;
			Inline_ = Other.Inline_;

			if (Other.Inline_)
			{
				Object_ = Storage_;
				Table_->Copy(Object_, Other.Object_);
			}
			else
			{
				Object_ = ::operator new(Table_->SizeValue);
				Table_->Copy(Object_, Other.Object_);
			}

			return *this;
		}

		Function(Function&& Other) noexcept
		{
			*this = Foundation::Utility::Move(Other);
		}

		auto operator=(Function&& Other) noexcept -> Function&
		{
			if (this == &Other)
			{
				return *this;
			}

			ResetInternal();

			Table_ = Other.Table_;
			Inline_ = Other.Inline_;

			if (!Other.Table_)
			{
				return *this;
			}

			if (Other.Inline_)
			{
				Object_ = Storage_;
				Table_->Move(Object_, Other.Object_);
				Other.ResetInternal();
			}
			else
			{
				Object_ = Other.Object_;
				Other.Object_ = nullptr;
				Other.Table_ = nullptr;
				Other.Inline_ = false;
			}

			return *this;
		}

		template<typename Callable, typename Stored = Foundation::Traits::RemoveCvReferenceType<Callable>, typename = Foundation::Traits::EnableIfType<!Foundation::Traits::IsSame<Stored, Function>::Value>>
		Function(Callable&& CallableObject)
		{
			Construct(Foundation::Utility::Forward<Callable>(CallableObject));
		}

		template<typename Callable, typename Stored = Foundation::Traits::RemoveCvReferenceType<Callable>, typename = Foundation::Traits::EnableIfType<!Foundation::Traits::IsSame<Stored, Function>::Value>>
		auto operator=(Callable&& CallableObject) -> Function&
		{
			ResetInternal();
			Construct(Foundation::Utility::Forward<Callable>(CallableObject));
			return *this;
		}

		auto operator=(Foundation::NullPtr) -> Function&
		{
			ResetInternal();
			return *this;
		}

		[[nodiscard]] explicit operator Foundation::Bool() const
		{
			return Table_ != nullptr;
		}

		[[nodiscard]] auto Empty() const -> Foundation::Bool
		{
			return Table_ == nullptr;
		}

		Foundation::Void Reset()
		{
			ResetInternal();
		}

		Return operator()(Args... Arguments) const
		{
			return Table_->Invoke(Object_, Foundation::Utility::Forward<Args>(Arguments)...);
		}
	};
}
