#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Optional.hpp>
#include <UEFIpp/UEFI/BootServices.hpp>
#include <UEFIpp/UEFI/Status.hpp>
#include <UEFIpp/UEFI/Types.hpp>
#include <UEFIpp/Protocols/Traits.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>
#include <UEFIpp/Stream/Output/Output.hpp>

namespace UEFIpp::Protocols
{
	class Access
	{
	public:
		using BootServicesType = UEFI::Table::BootServices;

	public:
		constexpr Access() = default;

		constexpr explicit Access(BootServicesType* BootServices) : BootServices_(BootServices)
		{
		}

		[[nodiscard]] constexpr auto Valid() const -> Foundation::Bool
		{
			return BootServices_ != nullptr;
		}

		[[nodiscard]] constexpr explicit operator Foundation::Bool() const
		{
			return Valid();
		}

		auto Reset(BootServicesType* BootServices) -> void
		{
			BootServices_ = BootServices;
		}

		[[nodiscard]] auto Raw() const -> BootServicesType*
		{
			return BootServices_;
		}

		template<typename TProtocol>
		[[nodiscard]] auto Locate() const -> Library::Optional<TProtocol*>
		{
			if (!BootServices_)
			{
				return Library::NullOpt;
			}

			Foundation::Void* Interface{};
			const auto Result = BootServices_->LocateProtocol(Foundation::Cast::Auto<UEFI::Guid*>(&ProtocolTraits<TProtocol>::Id), nullptr, &Interface);

			if (!UEFI::IsSuccess(Result))
			{
				return Library::NullOpt;
			}

			return Foundation::Cast::Auto<TProtocol*>(Interface);
		}

		template<typename TProtocol>
		[[nodiscard]] auto Handle(UEFI::Handle Handle) const -> Library::Optional<TProtocol*>
		{
			if (!BootServices_ || !Handle)
			{
				return Library::NullOpt;
			}

			Foundation::Void* Interface{};
			const auto Result = BootServices_->HandleProtocol(Handle, Foundation::Cast::Auto<UEFI::Guid*>(&ProtocolTraits<TProtocol>::Id), &Interface);

			if (!UEFI::IsSuccess(Result))
			{
				return Library::NullOpt;
			}

			return Foundation::Cast::Auto<TProtocol*>(Interface);
		}

		template<typename TProtocol>
		[[nodiscard]] auto LocateHandles() const -> Library::Optional<Library::Vector<UEFI::Handle>>
		{
			if (!BootServices_)
			{
				return Library::NullOpt;
			}

			Foundation::UintN BufferSize{};

			auto Result = BootServices_->LocateHandle(UEFI::Table::LocateSearchType::ByProtocol, Foundation::Cast::Auto<UEFI::Guid*>(&ProtocolTraits<TProtocol>::Id), nullptr, &BufferSize, nullptr);

			if (Result != UEFI::StatusCode::BufferTooSmall)
			{
				return Library::NullOpt;
			}

			Library::Vector<UEFI::Handle> Handles;
			Handles.Resize(BufferSize / sizeof(UEFI::Handle));

			Result = BootServices_->LocateHandle(UEFI::Table::LocateSearchType::ByProtocol, Foundation::Cast::Auto<UEFI::Guid*>(&ProtocolTraits<TProtocol>::Id), nullptr, &BufferSize, Handles.Data());

			if (!UEFI::IsSuccess(Result))
			{
				return Library::NullOpt;
			}

			return Handles;
		}

		template<typename TProtocol>
		[[nodiscard]] auto Install(UEFI::Handle* Handle, TProtocol* Interface) const -> UEFI::StatusCode
		{
			if (!BootServices_ || !Handle || !Interface)
			{
				return UEFI::StatusCode::InvalidParameter;
			}

			return BootServices_->InstallProtocolInterface(Handle, Foundation::Cast::Auto<UEFI::Guid*>(&ProtocolTraits<TProtocol>::Id), UEFI::Table::InterfaceType::NativeInterface, Interface);
		}

		template<typename TProtocol>
		[[nodiscard]] auto Uninstall(UEFI::Handle Handle, TProtocol* Interface) const -> UEFI::StatusCode
		{
			if (!BootServices_ || !Handle || !Interface)
			{
				return UEFI::StatusCode::InvalidParameter;
			}

			return BootServices_->UninstallProtocolInterface(Handle, Foundation::Cast::Auto<UEFI::Guid*>(&ProtocolTraits<TProtocol>::Id), Interface);
		}

	private:
		BootServicesType* BootServices_{};
	};
}