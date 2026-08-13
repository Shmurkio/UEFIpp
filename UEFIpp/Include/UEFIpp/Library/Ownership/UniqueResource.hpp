#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::Library
{
	template<typename TDeleter, typename TResource>
	concept ResourceDeleter = requires(
		TDeleter& Deleter,
		TResource& Resource)
	{
		Deleter(Resource);
	};

	template<typename TResource, typename TDeleter>
	requires ResourceDeleter<TDeleter, TResource>
	class UniqueResource
	{
	public:
		using ResourceType = TResource;
		using DeleterType = TDeleter;

		constexpr UniqueResource() noexcept = default;

		constexpr UniqueResource(
			ResourceType Resource,
			DeleterType Deleter
		) noexcept :
			Resource_(Foundation::Utility::Move(Resource)),
			Deleter_(Foundation::Utility::Move(Deleter)),
			Owns_(true)
		{
		}

		UniqueResource(const UniqueResource&) = delete;
		auto operator=(const UniqueResource&) -> UniqueResource& = delete;

		constexpr UniqueResource(UniqueResource&& Other) noexcept :
			Resource_(Foundation::Utility::Move(Other.Resource_)),
			Deleter_(Foundation::Utility::Move(Other.Deleter_)),
			Owns_(Foundation::Utility::Exchange(Other.Owns_, false))
		{
		}

		constexpr auto operator=(UniqueResource&& Other) noexcept -> UniqueResource&
		{
			if (this == &Other)
			{
				return *this;
			}

			Reset();
			Deleter_ = Foundation::Utility::Move(Other.Deleter_);
			Resource_ = Foundation::Utility::Move(Other.Resource_);
			Owns_ = Foundation::Utility::Exchange(Other.Owns_, false);
			return *this;
		}

		constexpr ~UniqueResource() noexcept
		{
			Reset();
		}

		[[nodiscard]] constexpr auto Get() noexcept -> ResourceType&
		{
			UEFIPP_ASSERT(Owns_);
			return Resource_;
		}

		[[nodiscard]] constexpr auto Get() const noexcept -> const ResourceType&
		{
			UEFIPP_ASSERT(Owns_);
			return Resource_;
		}

		[[nodiscard]] constexpr auto GetDeleter() noexcept -> DeleterType&
		{
			return Deleter_;
		}

		[[nodiscard]] constexpr auto GetDeleter() const noexcept -> const DeleterType&
		{
			return Deleter_;
		}

		[[nodiscard]] constexpr auto OwnsResource() const noexcept -> Foundation::Bool
		{
			return Owns_;
		}

		[[nodiscard]] constexpr explicit operator Foundation::Bool() const noexcept
		{
			return OwnsResource();
		}

		[[nodiscard]] constexpr auto Release() noexcept -> ResourceType
		{
			UEFIPP_ASSERT(Owns_);
			Owns_ = false;
			return Foundation::Utility::Move(Resource_);
		}

		constexpr auto Reset() noexcept -> Foundation::Void
		{
			if (!Owns_)
			{
				return;
			}

			Owns_ = false;
			(void)Deleter_(Resource_);
		}

		constexpr auto Reset(ResourceType Resource) noexcept -> Foundation::Void
		{
			Reset();
			Resource_ = Foundation::Utility::Move(Resource);
			Owns_ = true;
		}

		constexpr auto Swap(UniqueResource& Other) noexcept -> Foundation::Void
		{
			Foundation::Utility::Swap(Resource_, Other.Resource_);
			Foundation::Utility::Swap(Deleter_, Other.Deleter_);
			Foundation::Utility::Swap(Owns_, Other.Owns_);
		}

	private:
		ResourceType Resource_{};
		[[no_unique_address]] DeleterType Deleter_{};
		Foundation::Bool Owns_{};
	};

	template<typename TResource, typename TDeleter>
	UniqueResource(TResource, TDeleter) -> UniqueResource<TResource, TDeleter>;

	template<typename TResource, typename TDeleter>
	constexpr auto Swap(
		UniqueResource<TResource, TDeleter>& Left,
		UniqueResource<TResource, TDeleter>& Right
	) noexcept -> Foundation::Void
	{
		Left.Swap(Right);
	}
}
