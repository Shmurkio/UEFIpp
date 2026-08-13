#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::Memory {
template <typename T>
concept AllocatorBackend =
    requires(T &Backend, Foundation::Size NumberOfBytes,
             Foundation::Size Alignment, Foundation::Void *Address) {
      {
        Backend.Allocate(NumberOfBytes, Alignment)
      } -> Foundation::Concepts::Same<Foundation::Void *>;
      { Backend.Free(Address) } -> Foundation::Concepts::Same<Foundation::Bool>;
    };

class AllocatorStub {
public:
  using AllocateFunction = auto (*)(Foundation::Void *Context,
                                    Foundation::Size NumberOfBytes,
                                    Foundation::Size Alignment)
      -> Foundation::Void *;

  using FreeFunction = auto (*)(Foundation::Void *Context,
                                Foundation::Void *Address) -> Foundation::Bool;

  constexpr AllocatorStub() noexcept = default;

  constexpr AllocatorStub(Foundation::Void *Context, AllocateFunction Allocate,
                          FreeFunction Free) noexcept
      : Context_(Context), Allocate_(Allocate), Free_(Free) {}

  template <AllocatorBackend Backend>
    requires(!Foundation::Concepts::Same<
                Foundation::Traits::RemoveCvReferenceType<Backend>,
                AllocatorStub>)
  constexpr explicit AllocatorStub(Backend &Instance) noexcept
      : Context_(Foundation::Cast::Auto<Foundation::Void *>(&Instance)),
        Allocate_(&AllocateThunk<Backend>), Free_(&FreeThunk<Backend>) {}

  template <AllocatorBackend Backend>
    requires(!Foundation::Concepts::Same<
             Foundation::Traits::RemoveCvReferenceType<Backend>, AllocatorStub>)
  [[nodiscard]] static constexpr auto From(Backend &Instance) noexcept
      -> AllocatorStub {
    return AllocatorStub{Instance};
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
      -> AllocatorStub {
    return AllocatorStub{nullptr, &StatelessAllocateThunk<Allocate>,
                         &StatelessFreeThunk<Free>};
  }

  [[nodiscard]] auto
  Allocate(Foundation::Size NumberOfBytes,
           Foundation::Size Alignment = alignof(Foundation::UintPtr)) const
      -> Foundation::Void *;

  template <typename T>
  [[nodiscard]] auto AllocateStorage(Foundation::Size Count = 1) const -> T * {
    static_assert(Foundation::Concepts::Object<T>);

    if (!Count) {
      return nullptr;
    }

    constexpr auto MaximumSize = static_cast<Foundation::Size>(-1);

    if (Count > MaximumSize / sizeof(T)) {
      return nullptr;
    }

    return Foundation::Cast::Auto<T *>(Allocate(Count * sizeof(T), alignof(T)));
  }

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

static_assert(sizeof(AllocatorStub) == sizeof(Foundation::Void *) * 3);
static_assert(Foundation::Traits::IsStandardLayout<AllocatorStub>::Value);
static_assert(Foundation::Traits::IsTriviallyCopyable<AllocatorStub>::Value);
} // namespace UEFIpp::Memory
