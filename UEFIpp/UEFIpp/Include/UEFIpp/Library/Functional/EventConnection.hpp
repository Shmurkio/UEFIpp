#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::Library
{
	template<typename>
	class BasicEvent;

	class EventConnection
	{
		template<typename>
		friend class BasicEvent;

	private:
		Foundation::Uint64 Id_{};

	public:
		constexpr EventConnection() = default;

		constexpr explicit EventConnection(Foundation::Uint64 Id) : Id_(Id)
		{
		}

		[[nodiscard]] constexpr auto Id() const -> Foundation::Uint64
		{
			return Id_;
		}

		[[nodiscard]] constexpr auto Valid() const -> Foundation::Bool
		{
			return Id_ != 0;
		}

		constexpr explicit operator Foundation::Bool() const
		{
			return Valid();
		}

		constexpr auto Reset() -> Foundation::Void
		{
			Id_ = 0;
		}

		friend constexpr auto operator==(EventConnection Left, EventConnection Right) -> Foundation::Bool
		{
			return Left.Id_ == Right.Id_;
		}

		friend constexpr auto operator!=(EventConnection Left, EventConnection Right) -> Foundation::Bool
		{
			return !(Left == Right);
		}
	};
}
