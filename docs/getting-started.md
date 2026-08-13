# Getting started

This page takes you from a clean checkout to a linked `.efi` image and explains which project settings are essential rather than incidental.

## Requirements

The checked-in projects target Windows-hosted MSVC and x64 UEFI. You need:

- Visual Studio or Build Tools with the C++ toolchain that provides platform toolset `v145`;
- a Windows SDK;
- the MASM build customization installed with the C++ toolchain;
- an x64 UEFI environment, virtual machine, or machine on which to run the resulting image.

The code is freestanding at runtime. The build host is Windows, but the generated `.efi` files do not depend on the Windows runtime.

## Build the existing solution

Open `UEFIpp.slnx` and choose `Debug|x64` or `Release|x64`. From a Visual Studio developer shell, the equivalent command is:

```powershell
msbuild UEFIpp.slnx /m /p:Configuration=Debug /p:Platform=x64
```

The build produces:

```text
x64/Debug/UEFIpp.lib
x64/Debug/SampleApplication.efi
x64/Debug/SampleDxe.efi
```

Replace `Debug` with `Release` for optimized outputs.

## Start a new application or driver

The safest starting point is to copy the relevant sample project and give it a new project GUID and name:

- copy `Samples/SampleApplication` for an EFI application;
- copy `Samples/SampleDxe` for a boot-service driver.

Keep the adapter in `Source/EntryPoint.cpp` separate from your `Entry.cpp`. The adapter is firmware plumbing; `Entry.cpp` is the application or driver you actually maintain.

Your project should reference `UEFIpp/UEFIpp.vcxproj` with `LinkLibraryDependencies` enabled. It also needs `UEFIpp/Include` in `AdditionalIncludeDirectories`.

## Required compiler settings

The sample projects use these settings in both configurations:

| Setting | Value | Why |
| --- | --- | --- |
| Platform | `x64` | The architecture implementation and ABI are x64 |
| Language standard | `stdcpplatest` | The library uses modern concepts, attributes, and language features |
| Exceptions | disabled | There is no hosted exception runtime |
| RTTI | disabled | Avoids the hosted RTTI runtime and overhead |
| Runtime library | `MultiThreaded` | Keeps compiler support consistent with the static build |
| Buffer security check | disabled | EFI images do not link the normal security-cookie runtime |
| Additional options | `/Gs-` | Prevents stack-probe runtime dependencies |
| Warning level | level 4 | Matches the library and samples |

Debug adds `_DEBUG` and `UEFIPP_DEBUG`. Release adds `NDEBUG` and enables maximum-speed optimization.

## Required linker settings

An EFI image is not an ordinary Windows executable. Keep these settings:

| Setting | Application | Boot-service driver |
| --- | --- | --- |
| `TargetExt` | `.efi` | `.efi` |
| Subsystem | `EFI Application` | `EFI Boot Service Driver` |
| Entry-point symbol | `EfiMain` | `EfiMain` |
| Ignore all default libraries | `true` | `true` |
| Manifest generation | `false` | `false` |
| Incremental linking | `false` | `false` |
| Additional options | `/FIXED:NO /DYNAMICBASE:NO /NXCOMPAT:NO` | same |

The user-facing function is still named `Main`; the project-owned adapter exports `EfiMain` and performs context initialization. See [Applications and drivers](entry-points.md).

## Shared trace configuration

The repository root contains `Directory.Build.props`, which imports `UEFIpp/UEFIpp.props`. That property sheet defines:

```xml
<UEFIppTraceModule Condition="'$(UEFIppTraceModule)'==''">
  $(ProjectName)
</UEFIppTraceModule>
```

Every `Trace()` call therefore prints the current project name automatically. You can override it in a project if necessary:

```xml
<PropertyGroup>
  <UEFIppTraceModule>StorageDriver</UEFIppTraceModule>
</PropertyGroup>
```

Use an identifier-like value because the property is passed as a preprocessor token and stringized by the trace macro.

## Include styles

For application code, the umbrella header is convenient:

```cpp
#include <UEFIpp/UEFIpp.hpp>
```

It includes every public module and defines the namespace aliases used by the samples. For reusable components or headers, include only what you use:

```cpp
#include <UEFIpp/Library/Containers/Expected.hpp>
#include <UEFIpp/Library/String/String.hpp>
#include <UEFIpp/UEFI/Status.hpp>
```

Focused includes reduce coupling and make dependencies visible.

## Your first application

With the SampleApplication adapter in the project, `Entry.cpp` only needs this:

```cpp
#include <UEFIpp/UEFIpp.hpp>

[[nodiscard]] auto Main(
    const Vector<String>& Args
) -> UEFI::MainResult
{
    Stream::Out::Console
        << Stream::ClearScreen
        << "Application started" << Stream::Endl
        << "Arguments: " << Args.Size() << Stream::Endl;

    Stream::Out::Serial
        << Trace() << "Initialization complete" << Stream::Endl;

    return {};
}
```

Return `{}` for success. Return `MakeUnexpected(StatusCode)` for a firmware error:

```cpp
return MakeUnexpected(UEFI::StatusCode::OutOfResources);
```

## Your first driver

With the SampleDxe adapter, the driver-facing signature is:

```cpp
[[nodiscard]] auto Main() -> UEFI::MainResult
{
    Stream::Out::Serial
        << Trace() << "Driver loaded" << Stream::Endl;

    return {};
}
```

Register an unload callback through `Protocols::LoadedImage` if the driver supports unloading. The complete pattern is in [Applications and drivers](entry-points.md#driver-main-and-unload).

## Run the images

UEFIpp does not bundle a firmware emulator. Copy the `.efi` file into an EFI System Partition or a virtual FAT volume and start it from a UEFI shell or boot entry appropriate to your environment.

For an application in a UEFI shell:

```text
fs0:
SampleApplication.efi first "second argument"
```

The application adapter reads `LoadedImage::LoadOptions`, handles quotes, converts the UTF-16 command line to narrow strings, and passes a `Vector<String>` to `Main`.

## Next steps

- Read [Applications and drivers](entry-points.md) before changing either entry-point adapter.
- Read [Core programming model](programming-model.md) before designing ownership or error propagation.
- Use the [documentation home](README.md) to find a guide or module reference.
