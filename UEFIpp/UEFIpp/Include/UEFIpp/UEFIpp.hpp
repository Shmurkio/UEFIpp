#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Protocols/Protocols.hpp>
#include <UEFIpp/Stream/Stream.hpp>
#include <UEFIpp/UEFI/UEFI.hpp>
#include <UEFIpp/Architecture/Architecture.hpp>
#include <UEFIpp/Memory/Memory.hpp>
#include <UEFIpp/CRT/CRT.hpp>
#include <UEFIpp/Text/Text.hpp>
#include <UEFIpp/Library/Library.hpp>
#include <UEFIpp/FileSystem/FileSystem.hpp>
#include <UEFIpp/Reverse/Reverse.hpp>
#include <UEFIpp/Loader/Loader.hpp>
#include <UEFIpp/Time/Time.hpp>
#include <UEFIpp/Nvram/Nvram.hpp>
#include <UEFIpp/Crypto/Crypto.hpp>
#include <UEFIpp/Hooking/Hooking.hpp>
#include <UEFIpp/Bus/Bus.hpp>

namespace UEFI = UEFIpp::UEFI;
namespace Stream = UEFIpp::Stream;
namespace Foundation = UEFIpp::Foundation;
namespace Protocols = UEFIpp::Protocols;
namespace Architecture = UEFIpp::Architecture;
namespace X64 = Architecture::X64;
namespace Memory = UEFIpp::Memory;
namespace FileSystem = UEFIpp::FileSystem;
namespace Reverse = UEFIpp::Reverse;
namespace PeImage = UEFIpp::PeImage;
namespace Time = UEFIpp::Time;
namespace Nvram = UEFIpp::Nvram;
namespace Crypto = UEFIpp::Crypto;
namespace Loader = UEFIpp::Loader;
namespace FileSystem = UEFIpp::FileSystem;
namespace Hooking = UEFIpp::Hooking;
namespace Bus = UEFIpp::Bus;
using namespace UEFIpp::Library;