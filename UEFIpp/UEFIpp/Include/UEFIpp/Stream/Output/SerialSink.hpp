#pragma once

#include <UEFIpp/Stream/Output/Output.hpp>
#include <intrin.h>

namespace UEFIpp::Stream
{
	class SerialSink
	{
	public:
		static constexpr auto LineStatusOffset = Foundation::Uint16{ 5 };
		static constexpr auto TransmitterHoldingRegisterEmpty = Foundation::Bit::Mask<Foundation::Uint8>(5);

		constexpr SerialSink() = default;

		constexpr explicit SerialSink(Foundation::Uint16 Port) : Port_(Port), Enabled_(true)
		{
		}

		auto Write(const Foundation::Char* Data, Foundation::Size Length) -> void
		{
			if (!Enabled_)
			{
				return;
			}

			for (Foundation::Size Index = 0; Index < Length; ++Index)
			{
				while (!Foundation::Bit::IsSet(__inbyte(Port_ + LineStatusOffset), TransmitterHoldingRegisterEmpty))
				{
				}

				__outbyte(Port_, Data[Index]);
			}
		}

		auto Write(const Foundation::WChar* Data, Foundation::Size Length) -> void
		{
			if (!Data)
			{
				return;
			}

			for (Foundation::Size Index = 0; Index < Length; ++Index)
			{
				const auto C = Data[Index];

				if (C <= 0x7F)
				{
					const auto Narrow = Foundation::Cast::Auto<Foundation::Char>(C);
					Write(&Narrow, 1);
				}
				else
				{
					const auto Replacement = Foundation::Char{ '?' };
					Write(&Replacement, 1);
				}
			}
		}

		auto Clear() -> void
		{
			Write("\x1B[2J\x1B[H", 7);
		}

	private:
		Foundation::Uint16 Port_{};
		Foundation::Bool Enabled_{};
	};

	namespace Out
	{
		inline Output<SerialSink> Serial{};
	}
}