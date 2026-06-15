#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/UEFI/Context.hpp>
#include <UEFIpp/UEFI/RuntimeServices.hpp>
#include <UEFIpp/UEFI/Status.hpp>
#include <UEFIpp/Library/Containers/Optional.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>
#include <UEFIpp/Library/String/String.hpp>
#include <UEFIpp/Library/String/StringView.hpp>
#include <UEFIpp/Text/Encoding.hpp>
#include <UEFIpp/Memory/Memory.hpp>
#include <UEFIpp/Nvram/Attribute.hpp>
#include <UEFIpp/Nvram/VariableId.hpp>
#include <UEFIpp/Nvram/Variable.hpp>
#include <UEFIpp/Nvram/TypedVariable.hpp>
#include <UEFIpp/Nvram/QueryInfo.hpp>

namespace UEFIpp::Nvram
{
	class Store
	{
	public:
		using Byte = Foundation::Uint8;
		using SizeType = Foundation::Size;
		using DataType = Library::Vector<Byte>;
		using ByteSpan = Library::Span<const Byte>;

	public:
		Store() : RuntimeServices_(&UEFI::Context::RuntimeServices())
		{
		}

		explicit Store(UEFI::Table::RuntimeServices& RuntimeServices) : RuntimeServices_(&RuntimeServices)
		{
		}

		[[nodiscard]] auto Valid() const -> Foundation::Bool
		{
			return RuntimeServices_ && RuntimeServices_->GetVariable && RuntimeServices_->SetVariable && RuntimeServices_->GetNextVariableName;
		}

		[[nodiscard]] explicit operator Foundation::Bool() const
		{
			return Valid();
		}

		[[nodiscard]] auto Exists(const VariableId& Id) const -> Foundation::Bool
		{
			Foundation::UintN Size{};
			auto Attributes = Foundation::Uint32{};
			auto Name = NativeName(Id);
			auto Guid = Id.VendorGuid();

			const auto Status = RuntimeServices_->GetVariable(Name.Data(), &Guid, &Attributes, &Size, nullptr);

			return Status == UEFI::StatusCode::BufferTooSmall || Status == UEFI::StatusCode::Success;
		}

		[[nodiscard]] auto Read(const VariableId& Id) const -> Library::Optional<Variable>
		{
			if (!Valid())
			{
				return Library::NullOpt;
			}

			Foundation::UintN Size{};
			auto RawAttributes = Foundation::Uint32{};
			auto Name = NativeName(Id);
			auto Guid = Id.VendorGuid();

			auto Status = RuntimeServices_->GetVariable(Name.Data(), &Guid, &RawAttributes, &Size, nullptr);

			if (Status == UEFI::StatusCode::NotFound)
			{
				return Library::NullOpt;
			}

			if (Status != UEFI::StatusCode::BufferTooSmall && Status != UEFI::StatusCode::Success)
			{
				return Library::NullOpt;
			}

			DataType Data;

			if (!Data.Resize(Size))
			{
				return Library::NullOpt;
			}

			Status = RuntimeServices_->GetVariable(Name.Data(), &Guid, &RawAttributes, &Size, Data.Data());

			if (Status != UEFI::StatusCode::Success)
			{
				return Library::NullOpt;
			}

			Data.Resize(Size);

			return Variable{ Id, AttributeMask{ RawAttributes }, Foundation::Utility::Move(Data) };
		}

		[[nodiscard]] auto ReadTyped(const VariableId& Id) const -> Library::Optional<Variable>
		{
			return Read(Id);
		}

		[[nodiscard]] auto ReadBytes(const VariableId& Id) const -> Library::Optional<DataType>
		{
			auto Variable = Read(Id);

			if (!Variable)
			{
				return Library::NullOpt;
			}

			return Foundation::Utility::Move(Variable->Data());
		}

		template<typename T>
		[[nodiscard]] auto ReadValue(const VariableId& Id) const -> Library::Optional<T>
		{
			auto Variable = Read(Id);

			if (!Variable)
			{
				return Library::NullOpt;
			}

			return TypedVariable{ *Variable }.As<T>();
		}

		template<typename T>
		[[nodiscard]] auto ReadArray(const VariableId& Id) const -> Library::Optional<Library::Vector<T>>
		{
			auto Variable = Read(Id);

			if (!Variable || Variable->Size() % sizeof(T) != 0)
			{
				return Library::NullOpt;
			}

			const auto Count = Variable->Size() / sizeof(T);

			Library::Vector<T> Result;

			if (!Result.Resize(Count))
			{
				return Library::NullOpt;
			}

			Memory::Copy(Result.Data(), Variable->Data().Data(), Variable->Size());

			return Result;
		}

		[[nodiscard]] auto ReadBool(const VariableId& Id) const -> Library::Optional<Foundation::Bool>
		{
			auto Value = ReadValue<Foundation::Uint8>(Id);

			if (!Value)
			{
				return Library::NullOpt;
			}

			return *Value != 0;
		}

		[[nodiscard]] auto ReadUint8(const VariableId& Id) const -> Library::Optional<Foundation::Uint8>
		{
			return ReadValue<Foundation::Uint8>(Id);
		}

		[[nodiscard]] auto ReadUint16(const VariableId& Id) const -> Library::Optional<Foundation::Uint16>
		{
			return ReadValue<Foundation::Uint16>(Id);
		}

		[[nodiscard]] auto ReadUint32(const VariableId& Id) const -> Library::Optional<Foundation::Uint32>
		{
			return ReadValue<Foundation::Uint32>(Id);
		}

		[[nodiscard]] auto ReadUint64(const VariableId& Id) const -> Library::Optional<Foundation::Uint64>
		{
			return ReadValue<Foundation::Uint64>(Id);
		}

		[[nodiscard]] auto ReadUint16Array(const VariableId& Id) const -> Library::Optional<Library::Vector<Foundation::Uint16>>
		{
			return ReadArray<Foundation::Uint16>(Id);
		}

		auto WriteString(const VariableId& Id, Library::StringView Value, AttributeMask Attributes = DefaultAttributes) const -> UEFI::Status
		{
			return Write(Id, ByteSpan{ Foundation::Cast::Auto<const Byte*>(Value.Data()), Value.Size() }, Attributes);
		}

		auto WriteString(const VariableId& Id, const Library::String& Value, AttributeMask Attributes = DefaultAttributes) const -> UEFI::Status
		{
			return WriteString(Id, Value.View(), Attributes);
		}

		[[nodiscard]] auto ReadString(const VariableId& Id) const -> Library::Optional<Library::String>
		{
			auto Data = ReadBytes(Id);

			if (!Data)
			{
				return Library::NullOpt;
			}

			Library::String Result{};

			if (!Result.Resize(Data->Size()))
			{
				return Library::NullOpt;
			}

			Memory::Copy(Result.Data(), Data->Data(), Data->Size());

			return Result;
		}

		auto Write(const VariableId& Id, ByteSpan Data, AttributeMask Attributes = DefaultAttributes) const -> UEFI::Status
		{
			if (!Valid())
			{
				return UEFI::StatusCode::NotReady;
			}

			auto Name = NativeName(Id);
			auto Guid = Id.VendorGuid();

			return RuntimeServices_->SetVariable(Name.Data(), &Guid, Attributes.Value(), Data.Size(), Foundation::Cast::Auto<Foundation::Void*>(const_cast<Byte*>(Data.Data())));
		}

		auto Write(const VariableId& Id, const DataType& Data, AttributeMask Attributes = DefaultAttributes) const -> UEFI::Status
		{
			return Write(Id, ByteSpan{ Data.Data(), Data.Size() }, Attributes);
		}

		template<typename T>
		auto WriteValue(const VariableId& Id, const T& Value, AttributeMask Attributes = DefaultAttributes) const -> UEFI::Status
		{
			return Write(Id, ByteSpan{ Foundation::Cast::Auto<const Byte*>(&Value), sizeof(T) }, Attributes);
		}

		template<typename T>
		auto WriteArray(const VariableId& Id, Library::Span<const T> Values, AttributeMask Attributes = DefaultAttributes) const -> UEFI::Status
		{
			return Write(Id, ByteSpan{ Foundation::Cast::Auto<const Byte*>(Values.Data()), Values.SizeInBytes() }, Attributes);
		}

		auto WriteBool(const VariableId& Id, Foundation::Bool Value, AttributeMask Attributes = DefaultAttributes) const -> UEFI::Status
		{
			const auto Raw = Foundation::Cast::Auto<Foundation::Uint8>(Value ? 1 : 0);
			return WriteValue(Id, Raw, Attributes);
		}

		auto Append(const VariableId& Id, ByteSpan Data) const -> UEFI::Status
		{
			auto Existing = Read(Id);

			if (!Existing)
			{
				return Write(Id, Data, DefaultAttributes);
			}

			return Write(Id, Data, Existing->Attributes() | Attribute::AppendWrite);
		}

		auto Delete(const VariableId& Id) const -> UEFI::Status
		{
			if (!Valid())
			{
				return UEFI::StatusCode::NotReady;
			}

			auto Name = NativeName(Id);
			auto Guid = Id.VendorGuid();

			return RuntimeServices_->SetVariable(Name.Data(), &Guid, 0, 0, nullptr);
		}

		[[nodiscard]] auto Query(AttributeMask Attributes = DefaultAttributes) const -> Library::Optional<QueryInfo>
		{
			if (!RuntimeServices_ || !RuntimeServices_->QueryVariableInfo)
			{
				return Library::NullOpt;
			}

			Foundation::Uint64 MaximumStorageSize{};
			Foundation::Uint64 RemainingStorageSize{};
			Foundation::Uint64 MaximumVariableSize{};

			const auto Status = RuntimeServices_->QueryVariableInfo(Attributes.Value(), &MaximumStorageSize, &RemainingStorageSize, &MaximumVariableSize);

			if (Status != UEFI::StatusCode::Success)
			{
				return Library::NullOpt;
			}

			return QueryInfo{ MaximumStorageSize, RemainingStorageSize, MaximumVariableSize };
		}

		[[nodiscard]] auto Enumerate() const -> Library::Vector<VariableId>
		{
			Library::Vector<VariableId> Result;

			if (!Valid())
			{
				return Result;
			}

			Foundation::UintN NameCapacity = 128;
			Library::WideString Name;
			Name.Resize(NameCapacity);

			UEFI::Guid Guid{};

			for (;;)
			{
				auto NameSize = Foundation::Cast::Auto<Foundation::UintN>((NameCapacity + 1) * sizeof(Foundation::WChar));
				auto Status = RuntimeServices_->GetNextVariableName(&NameSize, Name.Data(), &Guid);

				if (Status == UEFI::StatusCode::NotFound)
				{
					break;
				}

				if (Status == UEFI::StatusCode::BufferTooSmall)
				{
					NameCapacity = NameSize / sizeof(Foundation::WChar);

					if (!Name.Resize(NameCapacity))
					{
						break;
					}

					Status = RuntimeServices_->GetNextVariableName(&NameSize, Name.Data(), &Guid);
				}

				if (Status != UEFI::StatusCode::Success)
				{
					break;
				}

				const auto CharacterCount = NameSize / sizeof(Foundation::WChar);

				if (CharacterCount == 0)
				{
					continue;
				}

				const auto VisibleCharacterCount = CharacterCount - 1;
				auto AsciiName = Text::Encoding::ToAscii(Library::WideStringView{ Name.Data(), VisibleCharacterCount });
				Result.PushBack(VariableId{ AsciiName.View(), Guid });
			}

			return Result;
		}

	private:
		[[nodiscard]] static auto NativeName(const VariableId& Id) -> Library::WideString
		{
			return Text::Encoding::ToWideAscii(Id.Name().View());
		}

	private:
		UEFI::Table::RuntimeServices* RuntimeServices_{};
	};
}