#pragma once

#include <UEFIpp/UEFI/Guid.hpp>
#include <UEFIpp/Protocols/SimpleTextInput.hpp>
#include <UEFIpp/Protocols/SimpleTextInputEx.hpp>
#include <UEFIpp/Protocols/SimpleTextOutput.hpp>
#include <UEFIpp/Protocols/SimpleFileSystem.hpp>
#include <UEFIpp/Protocols/File.hpp>
#include <UEFIpp/Protocols/LoadedImage.hpp>
#include <UEFIpp/Protocols/PciIo.hpp>
#include <UEFIpp/Protocols/Tcp4.hpp>
#include <UEFIpp/Protocols/ServiceBinding.hpp>
#include <UEFIpp/Protocols/Udp4.hpp>

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

	template<>
	class ProtocolTraits<PciIo>
	{
	public:
		inline static constexpr UEFI::Guid Id{ 0x4cf5b200, 0x68b8, 0x4ca5, { 0x9e, 0xec, 0xb2, 0x3e, 0x3f, 0x50, 0x02, 0x9a } };
	};

	template<>
	class ProtocolTraits<Tcp4ServiceBinding>
	{
	public:
		inline static constexpr UEFI::Guid Id{ 0x00720665, 0x67eb, 0x4a99, { 0xba, 0xf7, 0xd3, 0xc3, 0x3a, 0x1c, 0x7c, 0xc9 } };
	};

	template<>
	class ProtocolTraits<Tcp4>
	{
	public:
		inline static constexpr UEFI::Guid Id{ 0x65530bc7, 0xa359, 0x410f, { 0xb0, 0x10, 0x5a, 0xad, 0xc7, 0xec, 0x2b, 0x62 } };
	};

	template<>
	class ProtocolTraits<Udp4ServiceBinding>
	{
	public:
		inline static constexpr UEFI::Guid Id{ 0x83f01464, 0x99bd, 0x45e5, { 0xb3, 0x83, 0xaf, 0x63, 0x05, 0xd8, 0xe9, 0xe6 } };
	};

	template<>
	class ProtocolTraits<Udp4>
	{
	public:
		inline static constexpr UEFI::Guid Id{ 0x3ad9df29, 0x4501, 0x478d, { 0xb1, 0xf8, 0x7f, 0x7f, 0xe7, 0x0e, 0x50, 0xf3 } };
	};
}