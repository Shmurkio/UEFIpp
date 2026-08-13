#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::Memory {
template <typename T>
concept ExecutableAllocatorBackend =
    requires(T &Backend, Foundation::Size NumberOfBytes,
             Foundation::Size Alignment, Foundation::Void *Address) {
      {
        Backend.Allocate(NumberOfBytes, Alignment)
      } -> Foundation::Concepts::Same<Foundation::Void *>;
      { Backend.Free(Address) } -> Foundation::Concepts::Same<Foundation::Bool>;
    };

// Non-owning type-erased capability. Object-backed instances require the
// bound backend object to outlive every copy of the stub.
class ExecutableAllocatorStub {
public:
  using AllocateFunction = auto (*)(Foundation::Void *Context,
                                    Foundation::Size NumberOfBytes,
                                    Foundation::Size Alignment)
      -> Foundation::Void *;

  using FreeFunction = auto (*)(Foundation::Void *Context,
                                Foundation::Void *Address) -> Foundation::Bool;

  constexpr ExecutableAllocatorStub() noexcept = default;

  constexpr ExecutableAllocatorStub(Foundation::Void *Context,
                                    AllocateFunction Allocate,
                                    FreeFunction Free) noexcept
      : Context_(Context), Allocate_(Allocate), Free_(Free) {}

  template <ExecutableAllocatorBackend Backend>
    requires(!Foundation::Concepts::Same<
                Foundation::Traits::RemoveCvReferenceType<Backend>,
                ExecutableAllocatorStub>)
  constexpr explicit ExecutableAllocatorStub(Backend &Instance) noexcept
      : Context_(Foundation::Cast::Auto<Foundation::Void *>(&Instance)),
        Allocate_(&AllocateThunk<Backend>), Free_(&FreeThunk<Backend>) {}

  template <ExecutableAllocatorBackend Backend>
    requires(!Foundation::Concepts::Same<
             Foundation::Traits::RemoveCvReferenceType<Backend>,
             ExecutableAllocatorStub>)
  [[nodiscard]] static constexpr auto From(Backend &Instance) noexcept
      -> ExecutableAllocatorStub {
    return ExecutableAllocatorStub{Instance};
  }

  template <auto Allocate, auto Free>
    requires requires(Foundation::Size NumberOfBytes,
                      Foundation::Size Alignment, Foundation::Void *Address) {
      {
        Allocate(NumberOfBytes, Alignment)
      } -> Foundation::Concepts::Same<Foundation::Void *>;
      { Free(Address) } -> Foundation::Concepts::Same<Foundation::Bool>;
    }
  [[nodiscard]] static constexpr auto FromFunctions() noexcept
      -> ExecutableAllocatorStub {
    return ExecutableAllocatorStub{nullptr, &StatelessAllocateThunk<Allocate>,
                                   &StatelessFreeThunk<Free>};
  }

  [[nodiscard]] auto
  Allocate(Foundation::Size NumberOfBytes,
           Foundation::Size Alignment = alignof(Foundation::UintPtr)) const
      -> Foundation::Void *;

  [[nodiscard]] auto Free(Foundation::Void *Address) const -> Foundation::Bool;

  [[nodiscard]] constexpr auto IsValid() const noexcept -> Foundation::Bool {
    return Allocate_ && Free_;
  }

  [[nodiscard]] constexpr explicit operator Foundation::Bool() const noexcept {
    return IsValid();
  }

  [[nodiscard]] constexpr auto Context() const noexcept -> Foundation::Void * {
    return Context_;
  }

  constexpr auto Reset() noexcept -> Foundation::Void {
    Context_ = nullptr;
    Allocate_ = nullptr;
    Free_ = nullptr;
  }

private:
  template <auto Allocate>
  [[nodiscard]] static auto
  StatelessAllocateThunk(Foundation::Void *, Foundation::Size NumberOfBytes,
                         Foundation::Size Alignment) -> Foundation::Void * {
    return Allocate(NumberOfBytes, Alignment);
  }

  template <auto Free>
  static auto StatelessFreeThunk(Foundation::Void *, Foundation::Void *Address)
      -> Foundation::Bool {
    return Free(Address);
  }

  template <typename Backend>
  [[nodiscard]] static auto AllocateThunk(Foundation::Void *Context,
                                          Foundation::Size NumberOfBytes,
                                          Foundation::Size Alignment)
      -> Foundation::Void * {
    auto *Instance = Foundation::Cast::Auto<Backend *>(Context);
    return Instance->Allocate(NumberOfBytes, Alignment);
  }

  template <typename Backend>
  static auto FreeThunk(Foundation::Void *Context, Foundation::Void *Address)
      -> Foundation::Bool {
    auto *Instance = Foundation::Cast::Auto<Backend *>(Context);
    return Instance->Free(Address);
  }

  Foundation::Void *Context_{};
  AllocateFunction Allocate_{};
  FreeFunction Free_{};
};

static_assert(sizeof(ExecutableAllocatorStub) ==
              sizeof(Foundation::Void *) * 3);
static_assert(
    Foundation::Traits::IsStandardLayout<ExecutableAllocatorStub>::Value);
static_assert(
    Foundation::Traits::IsTriviallyCopyable<ExecutableAllocatorStub>::Value);
} // namespace UEFIpp::Memory
