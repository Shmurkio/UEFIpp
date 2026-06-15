#pragma once

#include <UEFIpp/UEFI/Guid.hpp>
#include <UEFIpp/Protocols/SimpleTextInput.hpp>
#include <UEFIpp/Protocols/SimpleTextInputEx.hpp>
#include <UEFIpp/Protocols/SimpleTextOutput.hpp>
#include <UEFIpp/Protocols/SimpleFileSystem.hpp>
#include <UEFIpp/Protocols/File.hpp>
#include <UEFIpp/Protocols/LoadedImage.hpp>

namespace UEFIpp::Protocols
{
	template<typename TProtocol>
	class ProtocolTraits;

	template<typename TProtocol>
	concept Protocol = requires
	{
		{ ProtocolTraits<TProtocol>::Id } -> Foundation::Concepts::Same<const UEFI::Guid&>;
	};

	template<>
	class ProtocolTraits<SimpleTextInput>
	{
	public:
		inline static constexpr UEFI::Guid Id{ 0x387477c1, 0x69c7, 0x11d2, { 0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b } };
	};

	template<>
	class ProtocolTraits<SimpleTextInputEx>
	{
	public:
		inline static constexpr UEFI::Guid Id{ 0xdd9e7534, 0x7762, 0x4698, { 0x8c, 0x14, 0xf5, 0x85, 0x17, 0xa6, 0x25, 0xaa } };
	};

	template<>
	class ProtocolTraits<SimpleTextOutput>
	{
	public:
		inline static constexpr UEFI::Guid Id{ 0x387477c2, 0x69c7, 0x11d2, { 0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b } };
	};

	template<>
	class ProtocolTraits<SimpleFileSystem>
	{
	public:
		inline static constexpr UEFI::Guid Id{ 0x964e5b22, 0x6459, 0x11d2, {0x8e, 0x39, 0x0, 0xa0, 0xc9, 0x69, 0x72, 0x3b } };
	};

	template<>
	class ProtocolTraits<File>
	{
	public:
		inline static constexpr UEFI::Guid Id{ 0x9576e91, 0x6d3f, 0x11d2, { 0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b } };
	};

	template<>
	class ProtocolTraits<LoadedImage>
	{
	public:
		inline static constexpr UEFI::Guid Id{ 0x5b1b31a1, 0x9562, 0x11d2, { 0x8e, 0x3f, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b } };
	};
}