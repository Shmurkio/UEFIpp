#pragma once

#include <UEFIpp/Foundation/Assertions.hpp>

namespace UEFIpp::Foundation
{
	template<typename T>
	struct IntrusiveListNode final
	{
		T* Previous{};
		T* Next{};
		Bool Linked{};
	};

	template<typename T, IntrusiveListNode<T> T::* TNode>
	class IntrusiveList final
	{
	public:
		constexpr IntrusiveList() noexcept = default;

		IntrusiveList(const IntrusiveList&) = delete;
		auto operator=(const IntrusiveList&) -> IntrusiveList& = delete;

		[[nodiscard]] constexpr auto Empty() const noexcept -> Bool
		{
			return Head_ == nullptr;
		}

		[[nodiscard]] constexpr auto Size() const noexcept -> Foundation::Size
		{
			return Size_;
		}

		[[nodiscard]] constexpr auto Front() const noexcept -> T*
		{
			return Head_;
		}

		[[nodiscard]] constexpr auto Back() const noexcept -> T*
		{
			return Tail_;
		}

		[[nodiscard]] auto PushBack(T& Value) noexcept -> Bool
		{
			auto& Node = Value.*TNode;
			if (Node.Linked)
			{
				return false;
			}

			Node.Previous = Tail_;
			Node.Next = nullptr;
			Node.Linked = true;

			if (Tail_)
			{
				(Tail_->*TNode).Next = &Value;
			}
			else
			{
				Head_ = &Value;
			}

			Tail_ = &Value;
			++Size_;
			return true;
		}

		[[nodiscard]] auto Remove(T& Value) noexcept -> Bool
		{
			auto& Node = Value.*TNode;
			if (!Node.Linked)
			{
				return false;
			}

			if (Node.Previous)
			{
				(Node.Previous->*TNode).Next = Node.Next;
			}
			else
			{
				Head_ = Node.Next;
			}

			if (Node.Next)
			{
				(Node.Next->*TNode).Previous = Node.Previous;
			}
			else
			{
				Tail_ = Node.Previous;
			}

			Node = {};
			UEFIPP_ASSERT(Size_ != 0);
			--Size_;
			return true;
		}

	private:
		T* Head_{};
		T* Tail_{};
		Foundation::Size Size_{};
	};
}
