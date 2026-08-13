#pragma once

#include <UEFIpp/Foundation/Types.hpp>

namespace UEFIpp::Foundation::Traits
{
	template<typename T, T V>
	struct Constant
	{
		using Type = Constant;

		static constexpr T Value = V;

		constexpr operator T() const
		{
			return Value;
		}
	};

	using True = Constant<bool, true>;
	using False = Constant<bool, false>;

	template<bool Value>
	using Bool = Constant<bool, Value>;

	template<typename T>
	struct TypeIdentity
	{
		using Type = T;
	};

	template<bool Condition, typename TrueType, typename FalseType>
	struct Conditional
	{
		using Type = TrueType;
	};

	template<typename TrueType, typename FalseType>
	struct Conditional<false, TrueType, FalseType>
	{
		using Type = FalseType;
	};

	template<bool Condition, typename TrueType, typename FalseType>
	using ConditionalType = typename Conditional<Condition, TrueType, FalseType>::Type;

	template<bool Condition, typename T = void>
	struct EnableIf
	{
	};

	template<typename T>
	struct EnableIf<true, T>
	{
		using Type = T;
	};

	template<bool Condition, typename T = void>
	using EnableIfType = typename EnableIf<Condition, T>::Type;

	template<typename T>
	struct RemoveConst
	{
		using Type = T;
	};

	template<typename T>
	struct RemoveConst<const T>
	{
		using Type = T;
	};

	template<typename T>
	struct RemoveVolatile
	{
		using Type = T;
	};

	template<typename T>
	struct RemoveVolatile<volatile T>
	{
		using Type = T;
	};

	template<typename T>
	struct RemoveCv
	{
		using Type = typename RemoveConst<typename RemoveVolatile<T>::Type>::Type;
	};

	template<typename T>
	struct RemoveReference
	{
		using Type = T;
	};

	template<typename T>
	struct RemoveReference<T&>
	{
		using Type = T;
	};

	template<typename T>
	struct RemoveReference<T&&>
	{
		using Type = T;
	};

	template<typename T>
	using RemoveConstType = typename RemoveConst<T>::Type;

	template<typename T>
	using RemoveVolatileType = typename RemoveVolatile<T>::Type;

	template<typename T>
	using RemoveCvType = typename RemoveCv<T>::Type;

	template<typename T>
	using RemoveReferenceType = typename RemoveReference<T>::Type;

	template<typename T>
	using RemoveCvReferenceType = RemoveCvType<RemoveReferenceType<T>>;

	template<typename A, typename B>
	struct IsSame : False
	{
	};

	template<typename T>
	struct IsSame<T, T> : True
	{
	};

	template<typename T>
	struct IsConst : False
	{
	};

	template<typename T>
	struct IsConst<const T> : True
	{
	};

	template<typename T>
	struct IsVolatile : False
	{
	};

	template<typename T>
	struct IsVolatile<volatile T> : True
	{
	};

	template<typename T>
	struct IsVoid : IsSame<RemoveCvType<T>, void>
	{
	};

	template<typename T>
	struct IsLValueReference : False
	{
	};

	template<typename T>
	struct IsLValueReference<T&> : True
	{
	};

	template<typename T>
	struct IsRValueReference : False
	{
	};

	template<typename T>
	struct IsRValueReference<T&&> : True
	{
	};

	template<typename T>
	struct IsReference : Bool<IsLValueReference<T>::Value || IsRValueReference<T>::Value>
	{
	};

	template<typename T>
	struct IsPointerBase : False
	{
	};

	template<typename T>
	struct IsPointerBase<T*> : True
	{
	};

	template<typename T>
	struct IsPointer : IsPointerBase<RemoveCvType<T>>
	{
	};

	template<typename T>
	struct RemovePointer
	{
		using Type = T;
	};

	template<typename T>
	struct RemovePointer<T*>
	{
		using Type = T;
	};

	template<typename T>
	struct RemovePointer<T* const>
	{
		using Type = T;
	};

	template<typename T>
	struct RemovePointer<T* volatile>
	{
		using Type = T;
	};

	template<typename T>
	struct RemovePointer<T* const volatile>
	{
		using Type = T;
	};

	template<typename T>
	using RemovePointerType = typename RemovePointer<T>::Type;

	template<typename T>
	struct IsIntegralBase : False
	{
	};

	template<> struct IsIntegralBase<bool> : True {};
	template<> struct IsIntegralBase<char> : True {};
	template<> struct IsIntegralBase<signed char> : True {};
	template<> struct IsIntegralBase<unsigned char> : True {};
	template<> struct IsIntegralBase<wchar_t> : True {};
	template<> struct IsIntegralBase<char8_t> : True {};
	template<> struct IsIntegralBase<char16_t> : True {};
	template<> struct IsIntegralBase<char32_t> : True {};
	template<> struct IsIntegralBase<short> : True {};
	template<> struct IsIntegralBase<unsigned short> : True {};
	template<> struct IsIntegralBase<int> : True {};
	template<> struct IsIntegralBase<unsigned int> : True {};
	template<> struct IsIntegralBase<long> : True {};
	template<> struct IsIntegralBase<unsigned long> : True {};
	template<> struct IsIntegralBase<long long> : True {};
	template<> struct IsIntegralBase<unsigned long long> : True {};

	template<typename T>
	struct IsIntegral : IsIntegralBase<RemoveCvType<T>>
	{
	};

	template<typename T>
	struct IsFloatingPointBase : False
	{
	};

	template<> struct IsFloatingPointBase<float> : True {};
	template<> struct IsFloatingPointBase<double> : True {};
	template<> struct IsFloatingPointBase<long double> : True {};

	template<typename T>
	struct IsFloatingPoint : IsFloatingPointBase<RemoveCvType<T>>
	{
	};

	template<typename T>
	struct IsArithmetic : Bool<IsIntegral<T>::Value || IsFloatingPoint<T>::Value>
	{
	};

	template<typename T>
	struct IsEnum : Bool<__is_enum(T)>
	{
	};

	template<typename T>
	struct IsClass : Bool<__is_class(T)>
	{
	};

	template<typename T>
	struct IsUnion : Bool<__is_union(T)>
	{
	};

	template<typename T>
	struct IsEmpty : Bool<__is_empty(T)>
	{
	};

	template<typename T>
	struct IsFinal : Bool<__is_final(T)>
	{
	};

	template<typename T>
	struct IsAbstract : Bool<__is_abstract(T)>
	{
	};

	template<typename T>
	struct IsPolymorphic : Bool<__is_polymorphic(T)>
	{
	};

	template<typename T>
	struct IsTriviallyCopyable : Bool<__is_trivially_copyable(T)>
	{
	};

	template<typename T>
	struct IsStandardLayout : Bool<__is_standard_layout(T)>
	{
	};

	template<typename T>
	struct IsScalar : Bool<
		IsArithmetic<T>::Value ||
		IsEnum<T>::Value ||
		IsPointer<T>::Value>
	{
	};

	template<typename T>
	struct IsObject : Bool<!IsVoid<T>::Value && !IsReference<T>::Value>
	{
	};
}