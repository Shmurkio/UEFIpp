#pragma once

#include <UEFIpp/Foundation/Traits.hpp>

namespace UEFIpp::Foundation::Concepts
{
	template<typename T>
	concept Void = Traits::IsVoid<T>::Value;

	template<typename T>
	concept Integral = Traits::IsIntegral<T>::Value;

	template<typename T>
	concept FloatingPoint = Traits::IsFloatingPoint<T>::Value;

	template<typename T>
	concept Arithmetic = Traits::IsArithmetic<T>::Value;

	template<typename T>
	concept Enum = Traits::IsEnum<T>::Value;

	template<typename T>
	concept Pointer = Traits::IsPointer<T>::Value;

	template<typename T>
	concept Reference = Traits::IsReference<T>::Value;

	template<typename T>
	concept LValueReference = Traits::IsLValueReference<T>::Value;

	template<typename T>
	concept RValueReference = Traits::IsRValueReference<T>::Value;

	template<typename T>
	concept Class = Traits::IsClass<T>::Value;

	template<typename T>
	concept Union = Traits::IsUnion<T>::Value;

	template<typename T>
	concept Empty = Traits::IsEmpty<T>::Value;

	template<typename T>
	concept Final = Traits::IsFinal<T>::Value;

	template<typename T>
	concept Abstract = Traits::IsAbstract<T>::Value;

	template<typename T>
	concept Polymorphic = Traits::IsPolymorphic<T>::Value;

	template<typename T>
	concept TriviallyCopyable = Traits::IsTriviallyCopyable<T>::Value;

	template<typename T>
	concept StandardLayout = Traits::IsStandardLayout<T>::Value;

	template<typename T>
	concept Scalar = Traits::IsScalar<T>::Value;

	template<typename T>
	concept Object = Traits::IsObject<T>::Value;

	template<typename T>
	concept Const = Traits::IsConst<T>::Value;

	template<typename T>
	concept Volatile = Traits::IsVolatile<T>::Value;

	template<typename A, typename B>
	concept Same = Traits::IsSame<A, B>::Value;
}