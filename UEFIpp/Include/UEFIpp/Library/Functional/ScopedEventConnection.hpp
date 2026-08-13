#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Functional/EventConnection.hpp>

namespace UEFIpp::Library
{
	template<typename EventType>
	class ScopedEventConnection
	{
	private:
		EventType* Event_{};
		EventConnection Connection_{};

	public:
		ScopedEventConnection() = default;

		ScopedEventConnection(EventType& Event, EventConnection Connection) : Event_(&Event), Connection_(Connection)
		{
		}

		~ScopedEventConnection()
		{
			Disconnect();
		}

		ScopedEventConnection(const ScopedEventConnection&) = delete;
		auto operator=(const ScopedEventConnection&) -> ScopedEventConnection & = delete;

		ScopedEventConnection(ScopedEventConnection&& Other) noexcept : Event_(Other.Event_), Connection_(Other.Connection_)
		{
			Other.Event_ = nullptr;
			Other.Connection_.Reset();
		}

		auto operator=(ScopedEventConnection&& Other) noexcept -> ScopedEventConnection&
		{
			if (this == &Other)
			{
				return *this;
			}

			Disconnect();

			Event_ = Other.Event_;
			Connection_ = Other.Connection_;

			Other.Event_ = nullptr;
			Other.Connection_.Reset();

			return *this;
		}

		auto Disconnect() -> Foundation::Bool
		{
			if (!Event_ || !Connection_)
			{
				return false;
			}

			auto Result = Event_->Disconnect(Connection_);

			Event_ = nullptr;
			Connection_.Reset();

			return Result;
		}

		[[nodiscard]] auto Connection() const -> EventConnection
		{
			return Connection_;
		}

		[[nodiscard]] auto Connected() const -> Foundation::Bool
		{
			return Event_ != nullptr && Connection_;
		}

		explicit operator Foundation::Bool() const
		{
			return Connected();
		}
	};
}
