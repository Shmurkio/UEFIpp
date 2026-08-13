#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Diagnostics/Diagnostics.hpp>
#include <UEFIpp/IO/IO.hpp>
#include <UEFIpp/Protocols/Protocols.hpp>
#include <UEFIpp/UEFI/UEFI.hpp>
#include <UEFIpp/Architecture/Architecture.hpp>
#include <UEFIpp/Memory/Memory.hpp>
#include <UEFIpp/CRT/CRT.hpp>
#include <UEFIpp/Library/Library.hpp>
#include <UEFIpp/FileSystem/FileSystem.hpp>
#include <UEFIpp/Reverse/Reverse.hpp>
#include <UEFIpp/Executable/Pe/Pe.hpp>
#include <sal.h>

namespace UEFI = UEFIpp::UEFI;
namespace IO = UEFIpp::IO;
namespace Foundation = UEFIpp::Foundation;
namespace Diagnostics = UEFIpp::Diagnostics;
namespace Protocols = UEFIpp::Protocols;
namespace Architecture = UEFIpp::Architecture;
namespace X64 = Architecture::X64;
namespace Memory = UEFIpp::Memory;
namespace FileSystem = UEFIpp::FileSystem;
namespace Reverse = UEFIpp::Reverse;
namespace Executable = UEFIpp::Executable;
namespace Pe = UEFIpp::Executable::Pe;
using namespace UEFIpp::Library;
