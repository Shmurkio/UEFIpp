#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::Nvram
{
	class QueryInfo
	{
	public:
		using SizeType = Foundation::Uint64;

	public:
		constexpr QueryInfo() = default;

		constexpr QueryInfo(SizeType MaximumStorageSize, SizeType RemainingStorageSize, SizeType MaximumVariableSize) : MaximumStorageSize_(MaximumStorageSize), RemainingStorageSize_(RemainingStorageSize), MaximumVariableSize_(MaximumVariableSize)
		{
		}

		[[nodiscard]] constexpr auto MaximumStorageSize() const -> SizeType
		{
			return MaximumStorageSize_;
		}

		[[nodiscard]] constexpr auto RemainingStorageSize() const -> SizeType
		{
			return RemainingStorageSize_;
		}

		[[nodiscard]] constexpr auto MaximumVariableSize() const -> SizeType
		{
			return MaximumVariableSize_;
		}

		[[nodiscard]] constexpr auto UsedStorageSize() const -> SizeType
		{
			return MaximumStorageSize_ - RemainingStorageSize_;
		}

	private:
		SizeType MaximumStorageSize_{};
		SizeType RemainingStorageSize_{};
		SizeType MaximumVariableSize_{};
	};
}