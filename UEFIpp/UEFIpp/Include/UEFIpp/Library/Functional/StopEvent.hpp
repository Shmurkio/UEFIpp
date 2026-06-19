#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Foundation/Utility.hpp>
#include <UEFIpp/Library/Functional/Function.hpp>
#include <UEFIpp/Library/Functional/EventResult.hpp>
#include <UEFIpp/Library/Functional/EventConnection.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>

namespace UEFIpp::Library
{
	template<typename... Args>
	class StopEvent
	{
	private:
		struct Entry
		{
			EventConnection Connection{};
			Function<EventResult(Args...)> Callback{};
			Foundation::Bool Enabled{ true };
			Foundation::Bool Once{ false };
			Foundation::Bool PendingRemove{ false };
		};

	private:
		Vector<Entry> Handlers_{};
		Foundation::Uint64 NextId_{ 1 };
		Foundation::Size EmitDepth_{};

	private:
		[[nodiscard]] auto NewConnection() -> EventConnection
		{
			return EventConnection{ NextId_++ };
		}

		auto Compact() -> Foundation::Void
		{
			for (Foundation::Size Index{}; Index < Handlers_.Size();)
			{
				if (Handlers_[Index].PendingRemove)
				{
					Handlers_.Erase(Index);
				}
				else
				{
					++Index;
				}
			}
		}

	public:
		StopEvent() = default;

		StopEvent(const StopEvent&) = delete;
		auto operator=(const StopEvent&) -> StopEvent & = delete;

		StopEvent(StopEvent&&) noexcept = default;
		auto operator=(StopEvent&&) noexcept -> StopEvent & = default;

		[[nodiscard]] auto Connect(Function<EventResult(Args...)> Callback) -> EventConnection
		{
			if (!Callback)
			{
				return {};
			}

			auto Connection = NewConnection();

			Handlers_.PushBack(Entry
			{
				Connection,
				Foundation::Utility::Move(Callback),
				true,
				false,
				false
			});

			return Connection;
		}

		[[nodiscard]] auto ConnectOnce(Function<EventResult(Args...)> Callback) -> EventConnection
		{
			if (!Callback)
			{
				return {};
			}

			auto Connection = NewConnection();

			Handlers_.PushBack(Entry
			{
				Connection,
				Foundation::Utility::Move(Callback),
				true,
				true,
				false
			});

			return Connection;
		}

		template<typename Callable>
		[[nodiscard]] auto Subscribe(Callable&& Callback) -> EventConnection
		{
			return Connect(Function<EventResult(Args...)>
			{
				Foundation::Utility::Forward<Callable>(Callback)
			});
		}

		template<typename Callable>
		[[nodiscard]] auto SubscribeOnce(Callable&& Callback) -> EventConnection
		{
			return ConnectOnce(Function<EventResult(Args...)>
			{
				Foundation::Utility::Forward<Callable>(Callback)
			});
		}

		auto Disconnect(EventConnection Connection) -> Foundation::Bool
		{
			if (!Connection)
			{
				return false;
			}

			for (Foundation::Size Index{}; Index < Handlers_.Size(); ++Index)
			{
				if (Handlers_[Index].Connection == Connection)
				{
					if (EmitDepth_ != 0)
					{
						Handlers_[Index].Enabled = false;
						Handlers_[Index].PendingRemove = true;
					}
					else
					{
						Handlers_.Erase(Index);
					}

					return true;
				}
			}

			return false;
		}

		auto DisconnectAll() -> Foundation::Void
		{
			if (EmitDepth_ != 0)
			{
				for (auto& Entry : Handlers_)
				{
					Entry.Enabled = false;
					Entry.PendingRemove = true;
				}
			}
			else
			{
				Handlers_.Clear();
			}
		}

		[[nodiscard]] auto Emit(Args... Arguments) -> EventResult
		{
			++EmitDepth_;

			EventResult Result = EventResult::Continue;

			for (auto& Entry : Handlers_)
			{
				if (!Entry.Enabled || Entry.PendingRemove)
				{
					continue;
				}

				Result = Entry.Callback(Foundation::Utility::Forward<Args>(Arguments)...);

				if (Entry.Once)
				{
					Entry.Enabled = false;
					Entry.PendingRemove = true;
				}

				if (Result == EventResult::Stop)
				{
					break;
				}
			}

			--EmitDepth_;

			if (EmitDepth_ == 0)
			{
				Compact();
			}

			return Result;
		}

		auto operator()(Args... Arguments) -> EventResult
		{
			return Emit(Foundation::Utility::Forward<Args>(Arguments)...);
		}

		auto Clear() -> Foundation::Void
		{
			DisconnectAll();
		}

		[[nodiscard]] auto Size() const -> Foundation::Size
		{
			Foundation::Size Count{};

			for (const auto& Entry : Handlers_)
			{
				if (!Entry.PendingRemove)
				{
					++Count;
				}
			}

			return Count;
		}

		[[nodiscard]] auto Empty() const -> Foundation::Bool
		{
			return Size() == 0;
		}

		[[nodiscard]] auto Contains(EventConnection Connection) const -> Foundation::Bool
		{
			for (const auto& Entry : Handlers_)
			{
				if (Entry.Connection == Connection && !Entry.PendingRemove)
				{
					return true;
				}
			}

			return false;
		}
	};
}