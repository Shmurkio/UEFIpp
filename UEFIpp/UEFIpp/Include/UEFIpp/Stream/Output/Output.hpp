#pragma once

#include <UEFIpp/Stream/Formatting/Common.hpp>

namespace UEFIpp::Stream
{
	template<typename TSink>
	class Output
	{
	public:
		static constexpr Foundation::Size BufferSize = 256;

		struct State
		{
			Base Base;
			HexCase HexCase;
			Foundation::UintN Width;
			Foundation::Char Fill;
			Foundation::Uint8 Precision;
		};

	public:
		constexpr Output() = default;

		constexpr explicit Output(TSink Sink) : Sink_(Sink)
		{
		}

		[[nodiscard]] auto SaveState() const -> State
		{
			return
			{
				Base_,
				HexCase_,
				Width_,
				Fill_,
				Precision_
			};
		}

		auto RestoreState(const State& Value) -> void
		{
			Base_ = Value.Base;
			HexCase_ = Value.HexCase;
			Width_ = Value.Width;
			Fill_ = Value.Fill;
			Precision_ = Value.Precision;
		}

		auto SetSink(TSink Sink) -> void
		{
			Flush();
			Sink_ = Sink;
		}

		auto Flush() -> void
		{
			if (!Length_)
			{
				return;
			}

			Sink_.Write(Buffer_, Length_);
			Length_ = 0;
		}

		auto Clear() -> void
		{
			Flush();
			Sink_.Clear();
		}

		auto Put(Foundation::Char C) -> void
		{
			if (Length_ == BufferSize)
			{
				Flush();
			}

			UEFIPP_ASSERT(Length_ < BufferSize);

			Buffer_[Length_++] = C;

			if (C == '\n')
			{
				Flush();
			}
		}

		auto Write(const Foundation::Char* String) -> void
		{
			if (!String)
			{
				Write("null");
				return;
			}

			while (*String)
			{
				auto C = *String++;

				if (C == '\n')
				{
					Put('\r');
					Put('\n');
				}
				else
				{
					Put(C);
				}
			}
		}

		auto Write(const Foundation::Char* String, Foundation::Size Size) -> void
		{
			if (!String)
			{
				Write("null");
				return;
			}

			for (Foundation::Size Index = 0; Index < Size; ++Index)
			{
				auto C = String[Index];

				if (C == '\n')
				{
					Put('\r');
					Put('\n');
				}
				else
				{
					Put(C);
				}
			}
		}

		auto Write(const Foundation::WChar* String) -> void
		{
			if (!String)
			{
				Write("null");
				return;
			}

			while (*String)
			{
				Write(String, 1);
				++String;
			}
		}

		auto Write(const Foundation::WChar* String, Foundation::Size Size) -> void
		{
			if (!String)
			{
				Write("null");
				return;
			}

			Flush();

			Sink_.Write(String, Size);
		}

		auto Unsigned(Foundation::Uint64 Value) -> void
		{
			if (Base_ == Base::Hex)
			{
				UnsignedHex(Value);
			}
			else
			{
				UnsignedDec(Value);
			}
		}

		auto Signed(Foundation::Int64 Value) -> void
		{
			if (Base_ == Base::Hex)
			{
				UnsignedHex(Foundation::Cast::Auto<Foundation::Uint64>(Value));
				return;
			}

			if (Value < 0)
			{
				Put('-');
				UnsignedDec(Foundation::Cast::Auto<Foundation::Uint64>(-(Value + 1)) + 1);
			}
			else
			{
				UnsignedDec(Foundation::Cast::Auto<Foundation::Uint64>(Value));
			}
		}

		auto Pointer(const Foundation::Void* Value) -> void
		{
			const auto State = SaveState();

			Base_ = Base::Hex;
			Width_ = sizeof(Foundation::UintPtr) * 2;
			Fill_ = '0';

			UnsignedHex(Foundation::Cast::PointerToAddress<Foundation::UintPtr>(Value));

			RestoreState(State);
		}

		auto Guid(const UEFI::Guid& Guid) -> void
		{
			const auto State = SaveState();

			Base_ = Base::Hex;
			Fill_ = '0';
			HexCase_ = HexCase::Upper;

			Width_ = 8;
			UnsignedHex(Guid.Data1);
			Put('-');

			Width_ = 4;
			UnsignedHex(Guid.Data2);
			Put('-');

			Width_ = 4;
			UnsignedHex(Guid.Data3);
			Put('-');

			const auto& Data4 = Guid.Data4;

			Width_ = 2;
			UnsignedHex(Data4[0]);

			Width_ = 2;
			UnsignedHex(Data4[1]);

			Put('-');

			for (Foundation::Size Index = 2; Index < 8; ++Index)
			{
				Width_ = 2;
				UnsignedHex(Data4[Index]);
			}

			RestoreState(State);
		}

		auto Floating(Foundation::Float64 Value) -> void
		{
			if (Base_ == Base::Hex)
			{
				Base_ = Base::Dec;
			}

			if (Value < 0.0)
			{
				Put('-');
				Value = -Value;
			}

			const auto Scale = Pow10(Precision_);
			const auto Scaled = Foundation::Cast::Auto<Foundation::Uint64>(
				Value * Foundation::Cast::Auto<Foundation::Float64>(Scale) + 0.5
			);

			const auto Integer = Scaled / Scale;
			const auto Fraction = Scaled % Scale;

			UnsignedDec(Integer);

			if (!Precision_)
			{
				return;
			}

			Put('.');

			auto Divisor = Scale / 10;

			while (Divisor)
			{
				const auto Digit = Foundation::Cast::Auto<Foundation::Uint8>((Fraction / Divisor) % 10);
				Put(Foundation::Cast::Auto<Foundation::Char>('0' + Digit));
				Divisor /= 10;
			}
		}

		auto SetBase(Base Value) -> void { Base_ = Value; }
		auto SetHexCase(HexCase Value) -> void { HexCase_ = Value; }
		auto SetWidth(Foundation::UintN Value) -> void { Width_ = Value; }
		auto SetFill(Foundation::Char Value) -> void { Fill_ = Value; }
		auto SetPrecision(Foundation::Uint8 Value) -> void { Precision_ = Value; }

	private:
		[[nodiscard]] auto HexDigit(Foundation::Uint8 Digit) const -> Foundation::Char
		{
			if (Digit < 10)
			{
				return Foundation::Cast::Auto<Foundation::Char>('0' + Digit);
			}

			return Foundation::Cast::Auto<Foundation::Char>((HexCase_ == HexCase::Upper ? 'A' : 'a') + (Digit - 10));
		}

		auto UnsignedDec(Foundation::Uint64 Value) -> void
		{
			if (!Value)
			{
				Put('0');
				return;
			}

			Foundation::Char Buffer[20]{};
			Foundation::Size Count = 0;

			while (Value)
			{
				Buffer[Count++] = Foundation::Cast::Auto<Foundation::Char>('0' + Value % 10);
				Value /= 10;
			}

			while (Count)
			{
				Put(Buffer[--Count]);
			}
		}

		auto UnsignedHex(Foundation::Uint64 Value) -> void
		{
			Foundation::Char Buffer[16]{};
			Foundation::Size Count = 0;

			if (!Value)
			{
				Buffer[Count++] = '0';
			}
			else
			{
				constexpr auto BitsPerHexDigit = Foundation::Uint64{ 4 };
				constexpr auto HexDigitMask = Foundation::Bit::LowMask<Foundation::Uint64>(BitsPerHexDigit);

				while (Value)
				{
					Buffer[Count++] = HexDigit(Foundation::Cast::Auto<Foundation::Uint8>(Value & HexDigitMask));
					Value >>= BitsPerHexDigit;
				}
			}

			while (Count < Width_)
			{
				Put(Fill_);
				--Width_;
			}

			while (Count)
			{
				Put(Buffer[--Count]);
			}

			Width_ = 0;
		}

		[[nodiscard]] static constexpr auto Pow10(Foundation::Uint8 Exponent) -> Foundation::Uint64
		{
			Foundation::Uint64 Result = 1;

			while (Exponent--)
			{
				Result *= 10;
			}

			return Result;
		}

	private:
		TSink Sink_{};
		Foundation::Char Buffer_[BufferSize]{};
		Foundation::Size Length_{};

		Base Base_{ Base::Dec };
		HexCase HexCase_{ HexCase::Upper };
		Foundation::UintN Width_{};
		Foundation::Char Fill_{ '0' };
		Foundation::Uint8 Precision_{ 6 };
	};

	template<typename TSink>
	auto operator<<(Output<TSink>& Stream, const Foundation::Char* String) -> Output<TSink>&
	{
		Stream.Write(String);
		return Stream;
	}

	template<typename TSink, typename TChar>
	auto operator<<(Output<TSink>& Stream, const Library::BasicString<TChar>& String) -> Output<TSink>&
	{
		return Stream << String.Data();
	}

	template<typename TSink>
	auto operator<<(Output<TSink>& Stream, Foundation::Char Value) -> Output<TSink>&
	{
		Stream.Put(Value);
		return Stream;
	}

	template<typename TSink>
	auto operator<<(Output<TSink>& Stream, Foundation::Bool Value) -> Output<TSink>&
	{
		return Stream << (Value ? "true" : "false");
	}

	template<typename TSink, Foundation::Concepts::Integral T>
	auto operator<<(Output<TSink>& Stream, T Value) -> Output<TSink>&
	{
		if constexpr (T(-1) < T(0))
		{
			Stream.Signed(Foundation::Cast::Auto<Foundation::Int64>(Value));
		}
		else
		{
			Stream.Unsigned(Foundation::Cast::Auto<Foundation::Uint64>(Value));
		}

		return Stream;
	}

	template<typename TSink>
	auto operator<<(Output<TSink>& Stream, const Foundation::Void* Value) -> Output<TSink>&
	{
		Stream.Pointer(Value);
		return Stream;
	}

	template<typename TSink>
	auto operator<<(Output<TSink>& Stream, const UEFI::Guid& Value) -> Output<TSink>&
	{
		Stream.Guid(Value);
		return Stream;
	}

	template<typename TSink>
	auto operator<<(Output<TSink>& Stream, EndLine) -> Output<TSink>&
	{
		Stream.Put('\r');
		Stream.Put('\n');
		Stream.Flush();
		return Stream;
	}

	template<typename TSink>
	auto operator<<(Output<TSink>& Stream, Flush) -> Output<TSink>&
	{
		Stream.Flush();
		return Stream;
	}

	template<typename TSink>
	auto operator<<(Output<TSink>& Stream, Hex) -> Output<TSink>&
	{
		Stream.SetBase(Base::Hex);
		return Stream;
	}

	template<typename TSink>
	auto operator<<(Output<TSink>& Stream, Dec) -> Output<TSink>&
	{
		Stream.SetBase(Base::Dec);
		return Stream;
	}

	template<typename TSink>
	auto operator<<(Output<TSink>& Stream, Upper) -> Output<TSink>&
	{
		Stream.SetHexCase(HexCase::Upper);
		return Stream;
	}

	template<typename TSink>
	auto operator<<(Output<TSink>& Stream, Lower) -> Output<TSink>&
	{
		Stream.SetHexCase(HexCase::Lower);
		return Stream;
	}

	template<typename TSink>
	auto operator<<(Output<TSink>& Stream, Width Value) -> Output<TSink>&
	{
		Stream.SetWidth(Value.Value);
		return Stream;
	}

	template<typename TSink>
	auto operator<<(Output<TSink>& Stream, Fill Value) -> Output<TSink>&
	{
		Stream.SetFill(Value.Value);
		return Stream;
	}

	template<typename TSink>
	auto operator<<(Output<TSink>& Stream, Precision Value) -> Output<TSink>&
	{
		Stream.SetPrecision(Value.Digits);
		return Stream;
	}

	template<typename TSink>
	auto operator<<(Output<TSink>& Stream, Foundation::Float32 Value) -> Output<TSink>&
	{
		Stream.Floating(Foundation::Cast::Auto<Foundation::Float64>(Value));
		return Stream;
	}

	template<typename TSink>
	auto operator<<(Output<TSink>& Stream, Foundation::Float64 Value) -> Output<TSink>&
	{
		Stream.Floating(Value);
		return Stream;
	}

	template<typename TSink>
	auto operator<<(Output<TSink>& Stream, Library::StringView View) -> Output<TSink>&
	{
		Stream.Write(View.Data(), View.Size());
		return Stream;
	}

	template<typename TSink>
	auto operator<<(Output<TSink>& Stream, Foundation::WChar Value) -> Output<TSink>&
	{
		Stream.Write(&Value, 1);
		return Stream;
	}

	template<typename TSink>
	auto operator<<(Output<TSink>& Stream, const Foundation::WChar* Text) -> Output<TSink>&
	{
		Stream.Write(Text);
		return Stream;
	}

	template<typename TSink>
	auto operator<<(Output<TSink>& Stream, const Library::U16String& String) -> Output<TSink>&
	{
		for (const auto Character : String)
		{
			Stream << Foundation::Cast::Auto<Foundation::WChar>(Character);
		}

		return Stream;
	}

	template<typename TSink>
	auto operator<<(Output<TSink>& Stream, Clear) -> Output<TSink>&
	{
		Stream.Clear();
		return Stream;
	}
}