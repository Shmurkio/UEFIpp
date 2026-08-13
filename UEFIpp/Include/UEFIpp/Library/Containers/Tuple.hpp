#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::Library {
template <typename... TValues>
class Tuple;

template <>
class Tuple<> final {
public:
  constexpr Tuple() noexcept = default;
};

template <typename TFirst, typename... TRest>
class Tuple<TFirst, TRest...> final {
public:
  constexpr Tuple() = default;

  template <typename UFirst, typename... URest>
  constexpr explicit Tuple(UFirst &&First, URest &&...Rest)
      : First_(Foundation::Utility::Forward<UFirst>(First)),
        Rest_(Foundation::Utility::Forward<URest>(Rest)...) {}

  [[nodiscard]] constexpr auto First() noexcept -> TFirst & { return First_; }
  [[nodiscard]] constexpr auto First() const noexcept -> const TFirst & {
    return First_;
  }
  [[nodiscard]] constexpr auto Rest() noexcept -> Tuple<TRest...> & {
    return Rest_;
  }
  [[nodiscard]] constexpr auto Rest() const noexcept -> const Tuple<TRest...> & {
    return Rest_;
  }

private:
  TFirst First_{};
  Tuple<TRest...> Rest_{};
};

namespace Detail {
template <typename T>
struct TupleDecay {
  using Type = Foundation::Traits::RemoveCvType<T>;
};
template <typename T, Foundation::Size N>
struct TupleDecay<T[N]> {
  using Type = T *;
};
template <typename T>
using TupleDecayType =
    typename TupleDecay<Foundation::Traits::RemoveReferenceType<T>>::Type;
} // namespace Detail

template <typename... TValues>
Tuple(TValues &&...) -> Tuple<Detail::TupleDecayType<TValues>...>;

template <Foundation::Size Index, typename TFirst, typename... TRest>
[[nodiscard]] constexpr decltype(auto) Get(Tuple<TFirst, TRest...> &Value) {
  if constexpr (Index == 0) return (Value.First());
  else return Get<Index - 1>(Value.Rest());
}

template <Foundation::Size Index, typename TFirst, typename... TRest>
[[nodiscard]] constexpr decltype(auto)
Get(const Tuple<TFirst, TRest...> &Value) {
  if constexpr (Index == 0) return (Value.First());
  else return Get<Index - 1>(Value.Rest());
}

template <typename T>
struct TupleSize;

template <typename... TValues>
struct TupleSize<Tuple<TValues...>>
    : Foundation::Traits::Constant<Foundation::Size, sizeof...(TValues)> {};

template <typename... TValues>
[[nodiscard]] constexpr auto MakeTuple(TValues &&...Values) {
  return Tuple<Detail::TupleDecayType<TValues>...>{
      Foundation::Utility::Forward<TValues>(Values)...};
}
} // namespace UEFIpp::Library
