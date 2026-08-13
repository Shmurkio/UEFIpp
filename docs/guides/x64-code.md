# x64 assembly, disassembly, and unwinding

The `Architecture::X64` module contains three related layers:

- an instruction model shared by encoding and decoding;
- a program/assembler pipeline that produces sectioned machine code and relocation metadata;
- a decoder, disassembler, formatter, and image unwinder for existing code.

These APIs are intended for x64 firmware. Privileged CPU helpers should only be used when the current execution context permits the operation.

## Namespaces

The umbrella header defines `X64 = UEFIpp::Architecture::X64`. The nested namespaces used most often are:

```cpp
namespace InstructionSet = X64::InstructionSet;
namespace Assembly = X64::Assembly;
namespace Disassembly = X64::Disassembly;
namespace Unwind = X64::Unwind;
```

## Build an instruction

`Instruction` stores a `Mnemonic`, up to five operands, and `EncodingOptions`:

```cpp
using namespace X64::InstructionSet;

Instruction Value{ Mnemonic::Mov };

if (!Value.AddOperand(Reg(Rax)) || !Value.AddOperand(Imm(42)))
{
    return MakeUnexpected(UEFI::StatusCode::OutOfResources);
}
```

Operand helpers include:

- `Reg(Register)`;
- `Imm`, `SImm`, `Relative`, and `Absolute`;
- `Mem(MemoryOperand)` and `Ptr(segment, offset)`;
- `BytePtr`, `WordPtr`, `DwordPtr`, and `QwordPtr`;
- `Address(base, index, scale, displacement, width)`;
- `AbsolutePtr` and `RipPtr`.

Decoded operands also carry visibility, read/write access, encoding, total width, element type, and element count.

## Assemble code

`Assembler` starts with a `.text` section. Convenience methods cover common instructions, and `Emit` accepts any modeled mnemonic:

```cpp
using namespace X64::InstructionSet;
using X64::Assembly::Assembler;

Assembler Code{};

if (!Code.Mov(Rax, Imm(42)))
{
    return MakeUnexpected(UEFI::StatusCode::OutOfResources);
}

if (!Code.Ret())
{
    return MakeUnexpected(UEFI::StatusCode::OutOfResources);
}

auto Result = Code.Assemble(0x100000);

if (!Result)
{
    // Inspect Result.Error(), an AssemblyError.
    return MakeUnexpected(UEFI::StatusCode::LoadError);
}

const auto Bytes = Result->Bytes();
```

The exact assembly failure is an `AssemblyError`; translate it only at a higher boundary that requires `StatusCode`.

### Labels and branches

```cpp
auto Loop = Code.CreateLabel();

if (!Loop || !Code.Bind(*Loop))
{
    return MakeUnexpected(UEFI::StatusCode::OutOfResources);
}

if (!Code.Dec(Rcx) || !Code.Jcc(Condition::NotZero, *Loop))
{
    return MakeUnexpected(UEFI::StatusCode::LoadError);
}
```

Labels belong to one `Program`. Bind each label once before final assembly. Branch width defaults to automatic selection; force a width only when the encoding contract requires it.

### Sections and data

`CreateSection(Kind, Name, Alignment)` returns a `SectionId`. Use `SwitchSection`, `EmitData`, and `Align` to build text, read-only data, data, or custom sections.

`AssembledCode` exposes:

- a contiguous `Bytes()` span;
- `Sections()` with IDs, kinds, offsets, sizes, and alignments;
- `Relocations()` for unresolved or externally meaningful references;
- the base address used for layout.

The bytes are data. They are not automatically placed in executable memory. If you intend to call generated code, allocate through an appropriate executable capability, copy the code, apply required relocation policy, and obey the platform's instruction-cache and memory-permission rules.

## Decode one instruction

```cpp
using namespace X64::Disassembly;

const Foundation::Byte CodeBytes[]
{
    0x48, 0xC7, 0xC0, 0x2A, 0x00, 0x00, 0x00
};

Decoder Decoder{};
auto Decoded = Decoder.Decode(CodeBytes, 0x100000);

if (!Decoded)
{
    // DecodeError explains why the bytes could not be decoded.
}
```

`Decoder` defaults to `MachineMode::Long64` and a 64-bit stack. Other machine modes and stack widths are available for data originating outside the normal x64 UEFI environment.

`DecodeAt(MemoryView, Address)` reads through a memory capability and returns `DecodeAtErrorInfo`, which distinguishes decode failure from memory-access failure.

## Disassemble a range

```cpp
Disassembler Disassembler{};
auto Listing = Disassembler.Disassemble(CodeBytes, 0x100000);

if (!Listing)
{
    return MakeUnexpected(UEFI::StatusCode::CompromisedData);
}

Formatter Formatter{ FormatterStyle::Intel };

for (const auto& Instruction : Listing->Instructions())
{
    auto Text = Formatter.Format(Instruction);

    if (Text)
    {
        Stream::Out::Console << *Text << Stream::Endl;
    }
}
```

`Disassembly` records the base address, total byte size, and decoded-instruction vector. `InstructionRange` is useful for iterating decoded instructions from a memory view when materializing a complete vector is not desirable.

`Formatter` supports Intel and AT&T syntax. `Format` allocates a `String`; `FormatTo` writes into a caller-provided character span and returns a `StringView` into that buffer.

## Decoded instruction data

`DecodedInstruction` contains the mnemonic, operands, length, runtime address, raw bytes, instruction attributes, CPU flag access, vector metadata, and raw legacy/REX/VEX/EVEX/MVEX encoding details.

Use high-level fields for control-flow and analysis. Use `RawEncodingInfo` only when the exact encoded byte layout matters.

`InstructionCategory`, `IsaSet`, `IsaExtension`, `InstructionEncoding`, `ControlFlowType`, and the `InstructionAttribute` mask let analysis code classify instructions without parsing mnemonics as text.

`RegisterSet` tracks sets of modeled registers. `Register` provides ID, class, width, root register, and string conversion. The named constants cover general-purpose, vector, tile, control, debug, mask, segment, bound, and special registers.

## CPU and register helpers

The architecture module also exposes:

- `Cpu::Cpuid` and feature checks;
- `CpuRandom` for hardware random generation where supported;
- `Msr` plus typed `FeatureControlMsr` and `PatMsr` helpers;
- wrappers for CR0, CR3, CR4, RFLAGS, EFER, and segment selectors.

Reading or writing control registers and MSRs is privileged and can fault or destabilize the machine. Check CPU support and reserved-bit requirements, and isolate writes behind platform-specific policy.

## Unwind an image

`Unwinder` reads the x64 PE exception directory from an in-memory image:

```cpp
X64::Unwind::Unwinder Unwinder{ ImageBase };

if (!Unwinder.Valid())
{
    return MakeUnexpected(UEFI::StatusCode::LoadError);
}

auto Caller = Unwinder.VirtualUnwind(CurrentContext);

if (!Caller)
{
    // Inspect UnwindError.
}
```

`FindRuntimeFunction` returns the runtime-function range containing an RIP. `VirtualUnwind` produces a caller `Context` and the stack slot holding the return address.

The unwinder expects a valid mapped PE image and supported Windows x64 unwind operations. It validates image and unwind metadata and returns `UnwindError` rather than guessing when data is unsupported.

## Common mistakes

- Do not discard `Expected` results from assembler or decoder calls.
- A successful assembly does not make memory executable.
- Labels are local to their program and must be bound.
- `FormatTo` returns a view into the supplied buffer.
- Runtime addresses affect relative target interpretation; pass the real base when it matters.
- Never write privileged registers merely because a typed wrapper makes it easy.

See [x64 architecture reference](../reference/architecture.md) for the complete type map.
