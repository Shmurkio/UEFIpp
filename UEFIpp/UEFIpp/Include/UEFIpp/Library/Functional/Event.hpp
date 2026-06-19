#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Foundation/Utility.hpp>
#include <UEFIpp/Library/Functional/Function.hpp>
#include <UEFIpp/Library/Functional/MoveOnlyFunction.hpp>
#include <UEFIpp/Library/Functional/EventConnection.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>

namespace UEFIpp::Library
{
	template<typename>
	class BasicEvent;

	template<typename Return, typename... Args>
	class BasicEvent<Return(Args...)>
	{
	public:
		using Handler = Function<Return(Args...)>;
		using MoveOnlyHandler = MoveOnlyFunction<Return(Args...)>;

	private:
		struct Entry
		{
			EventConnection Connection{};
			Handler Callback{};
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

		auto MarkRemove(EventConnection Connection) -> Foundation::Bool
		{
			if (!Connection)
				return false;

			for (auto& Entry : Handlers_)
			{
				if (Entry.Connection == Connection && !Entry.PendingRemove)
				{
					Entry.PendingRemove = true;
					Entry.Enabled = false;
					return true;
				}
			}

			return false;
		}

	public:
		BasicEvent() = default;

		BasicEvent(const BasicEvent&) = delete;
		auto operator=(const BasicEvent&) -> BasicEvent & = delete;

		BasicEvent(BasicEvent&&) noexcept = default;
		auto operator=(BasicEvent&&) noexcept -> BasicEvent & = default;

		~BasicEvent() = default;

		[[nodiscard]] auto Connect(Handler Callback) -> EventConnection
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

		[[nodiscard]] auto ConnectOnce(Handler Callback) -> EventConnection
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
			return Connect(Handler{ Foundation::Utility::Forward<Callable>(Callback) });
		}

		template<typename Callable>
		[[nodiscard]] auto SubscribeOnce(Callable&& Callback) -> EventConnection
		{
			return ConnectOnce(Handler{ Foundation::Utility::Forward<Callable>(Callback) });
		}

		auto Disconnect(EventConnection Connection) -> Foundation::Bool
		{
			if (EmitDepth_ != 0)
			{
				return MarkRemove(Connection);
			}

			if (!Connection)
			{
				return false;
			}

			for (Foundation::Size Index{}; Index < Handlers_.Size(); ++Index)
			{
				if (Handlers_[Index].Connection == Connection)
				{
					Handlers_.Erase(Index);
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

				return;
			}

			Handlers_.Clear();
		}

		auto Enable(EventConnection Connection) -> Foundation::Bool
		{
			for (auto& Entry : Handlers_)
			{
				if (Entry.Connection == Connection && !Entry.PendingRemove)
				{
					Entry.Enabled = true;
					return true;
				}
			}

			return false;
		}

		auto Disable(EventConnection Connection) -> Foundation::Bool
		{
			for (auto& Entry : Handlers_)
			{
				if (Entry.Connection == Connection && !Entry.PendingRemove)
				{
					Entry.Enabled = false;
					return true;
				}
			}

			return false;
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

		[[nodiscard]] auto Empty() const -> Foundation::Bool
		{
			return Size() == 0;
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

		[[nodiscard]] auto RawSize() const -> Foundation::Size
		{
			return Handlers_.Size();
		}

		auto Clear() -> Foundation::Void
		{
			DisconnectAll();
		}

		auto Emit(Args... Arguments) -> Foundation::Void
		{
			++EmitDepth_;

			for (auto& Entry : Handlers_)
			{
				if (!Entry.Enabled || Entry.PendingRemove)
				{
					continue;
				}

				Entry.Callback(Foundation::Utility::Forward<Args>(Arguments)...);

				if (Entry.Once)
				{
					Entry.Enabled = false;
					Entry.PendingRemove = true;
				}
			}

			--EmitDepth_;

			if (EmitDepth_ == 0)
			{
				Compact();
			}
		}

		auto operator()(Args... Arguments) -> Foundation::Void
		{
			Emit(Foundation::Utility::Forward<Args>(Arguments)...);
		}

		[[nodiscard]] auto operator+=(Handler Callback) -> EventConnection
		{
			return Connect(Foundation::Utility::Move(Callback));
		}

		auto operator-=(EventConnection Connection) -> Foundation::Bool
		{
			return Disconnect(Connection);
		}
	};

	template<typename... Args>
	using Event = BasicEvent<void(Args...)>;
}