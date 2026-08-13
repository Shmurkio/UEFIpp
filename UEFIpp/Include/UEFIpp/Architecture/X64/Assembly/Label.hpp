#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::Architecture::X64::Assembly
{
	class Label
	{
	public:
		static constexpr Foundation::Uint32 InvalidId =
			static_cast<Foundation::Uint32>(-1);

		constexpr Label() noexcept = default;

		constexpr explicit Label(
			Foundation::Uint32 Id
		) noexcept :
			Id_(Id)
		{
		}

		[[nodiscard]] constexpr auto Id() const noexcept -> Foundation::Uint32
		{
			return Id_;
		}

		[[nodiscard]] constexpr auto IsValid() const noexcept -> Foundation::Bool
		{
			return Id_ != InvalidId;
		}

		[[nodiscard]] constexpr auto operator==(
			const Label&
		) const noexcept -> Foundation::Bool = default;

	private:
		Foundation::Uint32 Id_{ InvalidId };
	};
}
