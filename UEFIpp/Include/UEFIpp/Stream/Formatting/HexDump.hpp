#pragma once

#include <UEFIpp/Stream/Output/Output.hpp>

namespace UEFIpp::Stream
{
	class HexDump
	{
	public:
		static constexpr Foundation::Size DefaultBytesPerLine = 16;

	public:
		constexpr HexDump(const Foundation::Void* Data, Foundation::Size Size, Foundation::Void* BaseAddress = nullptr) : Data_(Data), Size_(Size), BaseAddress_(BaseAddress)
		{
		}

		[[nodiscard]] constexpr auto Data() const -> const Foundation::Void*
		{
			return Data_;
		}

		[[nodiscard]] constexpr auto Size() const -> Foundation::Size
		{
			return Size_;
		}

		[[nodiscard]] constexpr auto BaseAddress() const -> Foundation::Void*
		{
			return BaseAddress_;
		}

		[[nodiscard]] constexpr auto BytesPerLine() const -> Foundation::Size
		{
			return BytesPerLine_;
		}

		[[nodiscard]] constexpr auto ShowAscii() const -> Foundation::Bool
		{
			return ShowAscii_;
		}

		[[nodiscard]] constexpr auto ShowAddress() const -> Foundation::Bool
		{
			return ShowAddress_;
		}

		constexpr auto BytesPerLine(Foundation::Size Value) -> HexDump&
		{
			BytesPerLine_ = Value ? Value : DefaultBytesPerLine;
			return *this;
		}

		constexpr auto ShowAscii(Foundation::Bool Value) -> HexDump&
		{
			ShowAscii_ = Value;
			return *this;
		}

		constexpr auto ShowAddress(Foundation::Bool Value) -> HexDump&
		{
			ShowAddress_ = Value;
			return *this;
		}

	private:
		const Foundation::Void* Data_{};
		Foundation::Size Size_{};
		Foundation::Void* BaseAddress_{};
		Foundation::Size BytesPerLine_{ DefaultBytesPerLine };
		Foundation::Bool ShowAscii_{ true };
		Foundation::Bool ShowAddress_{ true };
	};

	template<typename TSink>
	auto operator<<(Output<TSink>& Stream, const HexDump& Dump) -> Output<TSink>&
	{
		const auto State = Stream.SaveState();

		const auto* Bytes = static_cast<const Foundation::Uint8*>(Dump.Data());

		if (!Bytes)
		{
			Stream << "null";
			Stream.RestoreState(State);
			return Stream;
		}

		const auto BytesPerLine = Dump.BytesPerLine();

		for (Foundation::Size LineOffset = 0; LineOffset < Dump.Size(); LineOffset += BytesPerLine)
		{
			Stream << Hexadecimal << Uppercase << Fill{ '0' };

			if (Dump.ShowAddress())
			{
				Stream << Width{ sizeof(Foundation::UintPtr) * 2 } << Foundation::Cast::Auto<Foundation::UintPtr>(Dump.BaseAddress()) + LineOffset << "  ";
			}

			for (Foundation::Size Column = 0; Column < BytesPerLine; ++Column)
			{
				const auto Index = LineOffset + Column;

				if (Index < Dump.Size())
				{
					Stream << Width{ 2 } << Foundation::Cast::Auto<Foundation::Uint32>(Bytes[Index]) << ' ';
				}
				else
				{
					Stream << "   ";
				}

				if (Column == 7)
				{
					Stream << ' ';
				}
			}

			if (Dump.ShowAscii())
			{
				Stream << " |";

				for (Foundation::Size Column = 0; Column < BytesPerLine; ++Column)
				{
					const auto Index = LineOffset + Column;

					if (Index >= Dump.Size())
					{
						Stream << ' ';
						continue;
					}

					const auto Byte = Bytes[Index];

					if (Byte >= 0x20 && Byte <= 0x7E)
					{
						Stream << Foundation::Cast::Auto<Foundation::Char>(Byte);
					}
					else
					{
						Stream << '.';
					}
				}

				Stream << '|';
			}

			Stream << Endl;
		}

		Stream.RestoreState(State);

		return Stream;
	}
}