#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>
#include <UEFIpp/Nvram/Attribute.hpp>
#include <UEFIpp/Nvram/VariableId.hpp>

namespace UEFIpp::Nvram
{
	class Variable
	{
	public:
		using Byte = Foundation::Uint8;
		using DataType = Library::Vector<Byte>;

	public:
		Variable() = default;

		Variable(VariableId Id, AttributeMask Attributes, DataType Data) : Id_(Foundation::Utility::Move(Id)), Attributes_(Attributes), Data_(Foundation::Utility::Move(Data))
		{
		}

		[[nodiscard]] auto Id() const -> const VariableId&
		{
			return Id_;
		}

		[[nodiscard]] auto Attributes() const -> AttributeMask
		{
			return Attributes_;
		}

		[[nodiscard]] auto Data() const -> const DataType&
		{
			return Data_;
		}

		[[nodiscard]] auto Data() -> DataType&
		{
			return Data_;
		}

		[[nodiscard]] auto Empty() const -> Foundation::Bool
		{
			return Data_.Empty();
		}

		[[nodiscard]] auto Size() const -> Foundation::Size
		{
			return Data_.Size();
		}

	private:
		VariableId Id_{};
		AttributeMask Attributes_{};
		DataType Data_{};
	};
}