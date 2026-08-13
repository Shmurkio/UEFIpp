#pragma once

#include <UEFIpp/CRT/New.hpp>
#include <UEFIpp/IO/Core/Operations.hpp>
#include <UEFIpp/IO/Core/ReaderRef.hpp>
#include <UEFIpp/IO/Core/WriterRef.hpp>

namespace UEFIpp::IO {
template <Foundation::Size Capacity = sizeof(Foundation::Void *) * 8,
          Foundation::Size Alignment = 16>
class AnyWriter final {
public:
  constexpr AnyWriter() noexcept = default;

  template <typename TSink>
    requires OutputSink<Foundation::Traits::RemoveCvReferenceType<TSink>>
  explicit AnyWriter(TSink &&Sink) {
    Emplace<Foundation::Traits::RemoveCvReferenceType<TSink>>(
        Foundation::Utility::Forward<TSink>(Sink));
  }

  AnyWriter(const AnyWriter &) = delete;
  auto operator=(const AnyWriter &) -> AnyWriter & = delete;

  AnyWriter(AnyWriter &&Other) noexcept { MoveFrom(Other); }
  auto operator=(AnyWriter &&Other) noexcept -> AnyWriter & {
    if (this != &Other) {
      Reset();
      MoveFrom(Other);
    }
    return *this;
  }

  ~AnyWriter() { Reset(); }

  template <typename TSink, typename... TArguments>
    requires OutputSink<TSink>
  auto Emplace(TArguments &&...Arguments) -> TSink & {
    static_assert(sizeof(TSink) <= Capacity,
                  "I/O object exceeds AnyWriter inline capacity");
    static_assert(alignof(TSink) <= Alignment,
                  "I/O object exceeds AnyWriter inline alignment");
    Reset();
    ::new (Foundation::Cast::Pointer<Foundation::Void *>(Storage_))
        TSink(Foundation::Utility::Forward<TArguments>(Arguments)...);
    Table_ = &TableFor<TSink>;
    Closed_ = false;
    return *Foundation::Cast::Auto<TSink *>(
        Foundation::Cast::Pointer<Foundation::Void *>(Storage_));
  }

  auto Reset() noexcept -> Foundation::Void {
    if (!Table_) return;
    if (!Closed_) (void)Table_->Close(Storage_);
    Table_->Destroy(Storage_);
    Table_ = nullptr;
    Closed_ = false;
  }

  [[nodiscard]] constexpr auto Valid() const noexcept -> Foundation::Bool {
    return Table_ != nullptr && !Closed_;
  }
  [[nodiscard]] constexpr explicit operator Foundation::Bool() const noexcept {
    return Valid();
  }
  [[nodiscard]] auto WriteSome(Library::Span<const Foundation::Byte> Bytes)
      -> Result<Foundation::Size> {
    if (!Table_ || Closed_)
      return Failure(Error::Semantic(ErrorCode::Closed, Operation::Write));
    return Table_->Write(Storage_, Bytes);
  }
  [[nodiscard]] auto Flush() -> Result<> {
    if (!Table_ || Closed_)
      return Failure(Error::Semantic(ErrorCode::Closed, Operation::Flush));
    return Table_->Flush(Storage_);
  }
  [[nodiscard]] auto Close() -> Result<> {
    if (!Table_ || Closed_) return {};
    auto Result = Table_->Close(Storage_);
    if (Result) Closed_ = true;
    return Result;
  }
  [[nodiscard]] auto Ref() noexcept -> WriterRef { return WriterRef{*this}; }

private:
  struct VTable final {
    auto (*Write)(Foundation::Void *, Library::Span<const Foundation::Byte>)
        -> Result<Foundation::Size>;
    auto (*Flush)(Foundation::Void *) -> Result<>;
    auto (*Close)(Foundation::Void *) -> Result<>;
    auto (*Move)(Foundation::Void *, Foundation::Void *) -> Foundation::Void;
    auto (*Destroy)(Foundation::Void *) -> Foundation::Void;
  };

  template <typename TSink>
  inline static constexpr VTable TableFor{
      [](Foundation::Void *Object, Library::Span<const Foundation::Byte> Bytes)
          -> Result<Foundation::Size> {
        return Foundation::Cast::Auto<TSink *>(Object)->WriteSome(Bytes);
      },
      [](Foundation::Void *Object) -> Result<> {
        return IO::Flush(*Foundation::Cast::Auto<TSink *>(Object));
      },
      [](Foundation::Void *Object) -> Result<> {
        return IO::Close(*Foundation::Cast::Auto<TSink *>(Object));
      },
      [](Foundation::Void *Destination,
         Foundation::Void *Source) -> Foundation::Void {
        auto *Value = static_cast<TSink *>(Source);
        ::new (Destination) TSink(Foundation::Utility::Move(*Value));
        using Stored = TSink;
        Value->~Stored();
      },
      [](Foundation::Void *Object) -> Foundation::Void {
        using Stored = TSink;
        static_cast<Stored *>(Object)->~Stored();
      }};

  auto MoveFrom(AnyWriter &Other) noexcept -> Foundation::Void {
    Table_ = Other.Table_;
    if (!Table_) return;
    Table_->Move(Storage_, Other.Storage_);
    Closed_ = Other.Closed_;
    Other.Table_ = nullptr;
    Other.Closed_ = false;
  }

  alignas(Alignment) Foundation::Byte Storage_[Capacity]{};
  const VTable *Table_{};
  Foundation::Bool Closed_{};
};

template <Foundation::Size Capacity = sizeof(Foundation::Void *) * 8,
          Foundation::Size Alignment = 16>
class AnyReader final {
public:
  constexpr AnyReader() noexcept = default;

  template <typename TSource>
    requires InputSource<Foundation::Traits::RemoveCvReferenceType<TSource>>
  explicit AnyReader(TSource &&Source) {
    Emplace<Foundation::Traits::RemoveCvReferenceType<TSource>>(
        Foundation::Utility::Forward<TSource>(Source));
  }

  AnyReader(const AnyReader &) = delete;
  auto operator=(const AnyReader &) -> AnyReader & = delete;
  AnyReader(AnyReader &&Other) noexcept { MoveFrom(Other); }
  auto operator=(AnyReader &&Other) noexcept -> AnyReader & {
    if (this != &Other) {
      Reset();
      MoveFrom(Other);
    }
    return *this;
  }
  ~AnyReader() { Reset(); }

  template <typename TSource, typename... TArguments>
    requires InputSource<TSource>
  auto Emplace(TArguments &&...Arguments) -> TSource & {
    static_assert(sizeof(TSource) <= Capacity,
                  "I/O object exceeds AnyReader inline capacity");
    static_assert(alignof(TSource) <= Alignment,
                  "I/O object exceeds AnyReader inline alignment");
    Reset();
    ::new (Foundation::Cast::Pointer<Foundation::Void *>(Storage_))
        TSource(Foundation::Utility::Forward<TArguments>(Arguments)...);
    Table_ = &TableFor<TSource>;
    Closed_ = false;
    return *Foundation::Cast::Auto<TSource *>(
        Foundation::Cast::Pointer<Foundation::Void *>(Storage_));
  }

  auto Reset() noexcept -> Foundation::Void {
    if (!Table_) return;
    if (!Closed_) (void)Table_->Close(Storage_);
    Table_->Destroy(Storage_);
    Table_ = nullptr;
    Closed_ = false;
  }
  [[nodiscard]] constexpr auto Valid() const noexcept -> Foundation::Bool {
    return Table_ != nullptr && !Closed_;
  }
  [[nodiscard]] constexpr explicit operator Foundation::Bool() const noexcept {
    return Valid();
  }
  [[nodiscard]] auto ReadSome(Library::Span<Foundation::Byte> Bytes)
      -> Result<ReadCount> {
    if (!Table_ || Closed_)
      return Failure(Error::Semantic(ErrorCode::Closed, Operation::Read));
    return Table_->Read(Storage_, Bytes);
  }
  [[nodiscard]] auto Close() -> Result<> {
    if (!Table_ || Closed_) return {};
    auto Result = Table_->Close(Storage_);
    if (Result) Closed_ = true;
    return Result;
  }
  [[nodiscard]] auto Ref() noexcept -> ReaderRef { return ReaderRef{*this}; }

private:
  struct VTable final {
    auto (*Read)(Foundation::Void *, Library::Span<Foundation::Byte>)
        -> Result<ReadCount>;
    auto (*Close)(Foundation::Void *) -> Result<>;
    auto (*Move)(Foundation::Void *, Foundation::Void *) -> Foundation::Void;
    auto (*Destroy)(Foundation::Void *) -> Foundation::Void;
  };

  template <typename TSource>
  inline static constexpr VTable TableFor{
      [](Foundation::Void *Object, Library::Span<Foundation::Byte> Bytes)
          -> Result<ReadCount> {
        return Foundation::Cast::Auto<TSource *>(Object)->ReadSome(Bytes);
      },
      [](Foundation::Void *Object) -> Result<> {
        return IO::Close(*Foundation::Cast::Auto<TSource *>(Object));
      },
      [](Foundation::Void *Destination,
         Foundation::Void *Source) -> Foundation::Void {
        auto *Value = static_cast<TSource *>(Source);
        ::new (Destination) TSource(Foundation::Utility::Move(*Value));
        using Stored = TSource;
        Value->~Stored();
      },
      [](Foundation::Void *Object) -> Foundation::Void {
        using Stored = TSource;
        static_cast<Stored *>(Object)->~Stored();
      }};

  auto MoveFrom(AnyReader &Other) noexcept -> Foundation::Void {
    Table_ = Other.Table_;
    if (!Table_) return;
    Table_->Move(Storage_, Other.Storage_);
    Closed_ = Other.Closed_;
    Other.Table_ = nullptr;
    Other.Closed_ = false;
  }

  alignas(Alignment) Foundation::Byte Storage_[Capacity]{};
  const VTable *Table_{};
  Foundation::Bool Closed_{};
};
} // namespace UEFIpp::IO
