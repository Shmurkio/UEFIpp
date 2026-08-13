#pragma once

#include <UEFIpp/IO/Core/Operations.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>
#include <UEFIpp/Memory/Memory.hpp>

namespace UEFIpp::IO {
class NullSink final {
public:
  [[nodiscard]] constexpr auto
  WriteSome(Library::Span<const Foundation::Byte> Bytes)
      -> Result<Foundation::Size> {
    return Bytes.Size();
  }
};

class CountingSink final {
public:
  [[nodiscard]] constexpr auto
  WriteSome(Library::Span<const Foundation::Byte> Bytes)
      -> Result<Foundation::Size> {
    Count_ += Bytes.Size();
    return Bytes.Size();
  }

  [[nodiscard]] constexpr auto Count() const noexcept -> Foundation::Size {
    return Count_;
  }

private:
  Foundation::Size Count_{};
};

class FixedBufferSink final {
public:
  constexpr explicit FixedBufferSink(Library::Span<Foundation::Byte> Buffer)
      : Buffer_(Buffer) {}

  [[nodiscard]] auto WriteSome(Library::Span<const Foundation::Byte> Bytes)
      -> Result<Foundation::Size> {
    const auto Available = Buffer_.Size() - Size_;
    if (!Available && !Bytes.Empty()) {
      return Failure(Error::Semantic(ErrorCode::Overflow, Operation::Write,
                                     Size_, Size_));
    }
    const auto Count = Bytes.Size() < Available ? Bytes.Size() : Available;
    Memory::Copy(Buffer_.Data() + Size_, Bytes.Data(), Count);
    Size_ += Count;
    return Count;
  }

  [[nodiscard]] constexpr auto Size() const noexcept -> Foundation::Size {
    return Size_;
  }

  [[nodiscard]] constexpr auto Written() const noexcept
      -> Library::Span<const Foundation::Byte> {
    return {Buffer_.Data(), Size_};
  }

private:
  Library::Span<Foundation::Byte> Buffer_{};
  Foundation::Size Size_{};
};

class MemorySink final {
public:
  constexpr explicit MemorySink(Memory::AllocatorStub Allocator = {})
      : Buffer_(Allocator) {}

  [[nodiscard]] auto WriteSome(Library::Span<const Foundation::Byte> Bytes)
      -> Result<Foundation::Size> {
    const auto OldSize = Buffer_.Size();
    if (!Buffer_.Resize(OldSize + Bytes.Size())) {
      return Failure(
          Error::Semantic(ErrorCode::OutOfMemory, Operation::Allocate));
    }
    Memory::Copy(Buffer_.Data() + OldSize, Bytes.Data(), Bytes.Size());
    return Bytes.Size();
  }

  [[nodiscard]] constexpr auto Buffer() const noexcept
      -> const Library::Vector<Foundation::Byte> & {
    return Buffer_;
  }

  auto Clear() -> Foundation::Void { Buffer_.Clear(); }

private:
  Library::Vector<Foundation::Byte> Buffer_{};
};

class MemorySource final {
public:
  constexpr MemorySource() = default;
  constexpr explicit MemorySource(Library::Span<const Foundation::Byte> Data)
      : Data_(Data) {}

  [[nodiscard]] auto ReadSome(Library::Span<Foundation::Byte> Destination)
      -> Result<ReadCount> {
    const auto Remaining = Data_.Size() - Position_;
    const auto Count = Destination.Size() < Remaining ? Destination.Size()
                                                       : Remaining;
    if (Count) {
      Memory::Copy(Destination.Data(), Data_.Data() + Position_, Count);
    }
    Position_ += Count;
    return ReadCount{Count, Position_ == Data_.Size()};
  }

  [[nodiscard]] auto Seek(Foundation::Uint64 Position)
      -> Result<Foundation::Uint64> {
    if (Position > Data_.Size()) {
      return Failure(
          Error::Semantic(ErrorCode::InvalidArgument, Operation::Seek));
    }
    Position_ = Foundation::Cast::Auto<Foundation::Size>(Position);
    return Position;
  }

  [[nodiscard]] auto Tell() -> Result<Foundation::Uint64> { return Position_; }

private:
  Library::Span<const Foundation::Byte> Data_{};
  Foundation::Size Position_{};
};

template <OutputSink TFirst, OutputSink TSecond>
class TeeSink final {
public:
  constexpr TeeSink(TFirst First, TSecond Second)
      : First_(Foundation::Utility::Move(First)),
        Second_(Foundation::Utility::Move(Second)) {}

  [[nodiscard]] auto WriteSome(Library::Span<const Foundation::Byte> Bytes)
      -> Result<Foundation::Size> {
    auto FirstResult = WriteAll(First_, Bytes);
    if (!FirstResult) {
      return Failure(FirstResult.Error());
    }
    auto SecondResult = WriteAll(Second_, Bytes);
    if (!SecondResult) {
      return Failure(SecondResult.Error());
    }
    return Bytes.Size();
  }

  [[nodiscard]] auto Flush() -> Result<> {
    auto FirstResult = IO::Flush(First_);
    auto SecondResult = IO::Flush(Second_);
    if (!FirstResult) return FirstResult;
    return SecondResult;
  }

  [[nodiscard]] auto Close() -> Result<> {
    auto FirstResult = IO::Close(First_);
    auto SecondResult = IO::Close(Second_);
    if (!FirstResult) return FirstResult;
    return SecondResult;
  }

private:
  TFirst First_;
  TSecond Second_;
};
} // namespace UEFIpp::IO
