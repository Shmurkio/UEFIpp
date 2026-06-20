#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>
#include <UEFIpp/Library/Functional/Event.hpp>
#include <UEFIpp/Library/String/String.hpp>
#include <UEFIpp/Library/String/StringView.hpp>
#include <UEFIpp/Network/FTP/FtpEntry.hpp>
#include <UEFIpp/Network/FTP/FtpResponse.hpp>
#include <UEFIpp/Network/IPv4Address.hpp>
#include <UEFIpp/Network/Transport/Tcp4Client.hpp>
#include <UEFIpp/UEFI/Status.hpp>

namespace UEFIpp::Network
{
    enum class FtpTransferType : Foundation::Byte
    {
        Ascii,
        Binary
    };

    class FtpClient
    {
    public:
        using ByteSpan = Library::Span<const Foundation::Byte>;

    public:
        Library::Event<> OnConnected{};
        Library::Event<> OnDisconnected{};
        Library::Event<> OnLoggedIn{};
        Library::Event<> OnTransferStarted{};
        Library::Event<> OnTransferFinished{};

        Library::Event<UEFI::StatusCode> OnError{};
        Library::Event<Library::StringView> OnResolvingHost{};
        Library::Event<const IPv4Address&> OnHostResolved{};
        Library::Event<Library::StringView> OnCommandSent{};
        Library::Event<const FtpResponse&> OnResponseReceived{};
        Library::Event<const IPv4Address&, Foundation::Uint16> OnPassiveModeEntered{};
        Library::Event<ByteSpan> OnDataReceived{};
        Library::Event<ByteSpan> OnDataSent{};
        Library::Event<const FtpEntry&> OnEntryReceived{};

    public:
        FtpClient() = default;
        ~FtpClient();

        FtpClient(const FtpClient&) = delete;
        auto operator=(const FtpClient&) -> FtpClient & = delete;

        FtpClient(FtpClient&& Other) noexcept;
        auto operator=(FtpClient&& Other) noexcept -> FtpClient&;

        [[nodiscard]] auto Connect(const IPv4Address& Address, Foundation::Uint16 Port = DefaultPort) -> Foundation::Bool;
        [[nodiscard]] auto Connect(Library::StringView Host, Foundation::Uint16 Port = DefaultPort) -> Foundation::Bool;

        [[nodiscard]] auto Login(Library::StringView Username, Library::StringView Password) -> Foundation::Bool;
        [[nodiscard]] auto LoginAnonymous() -> Foundation::Bool;

        [[nodiscard]] auto SetTransferType(FtpTransferType Type) -> Foundation::Bool;
        [[nodiscard]] auto SetBinary() -> Foundation::Bool;
        [[nodiscard]] auto SetAscii() -> Foundation::Bool;

        [[nodiscard]] auto PrintWorkingDirectory(Library::String& Path) -> Foundation::Bool;
        [[nodiscard]] auto ChangeDirectory(Library::StringView Path) -> Foundation::Bool;
        [[nodiscard]] auto MakeDirectory(Library::StringView Path) -> Foundation::Bool;
        [[nodiscard]] auto RemoveDirectory(Library::StringView Path) -> Foundation::Bool;
        [[nodiscard]] auto DeleteFile(Library::StringView Path) -> Foundation::Bool;

        [[nodiscard]] auto List(Library::Vector<FtpEntry>& Entries, Library::StringView Path = {}) -> Foundation::Bool;
        [[nodiscard]] auto ListRaw(Library::String& Text, Library::StringView Path = {}) -> Foundation::Bool;

        [[nodiscard]] auto Download(Library::StringView Path, Library::Vector<Foundation::Byte>& Data) -> Foundation::Bool;
        [[nodiscard]] auto DownloadText(Library::StringView Path, Library::String& Text) -> Foundation::Bool;
        [[nodiscard]] auto Upload(Library::StringView Path, ByteSpan Data) -> Foundation::Bool;
        [[nodiscard]] auto UploadText(Library::StringView Path, Library::StringView Text) -> Foundation::Bool;

        [[nodiscard]] auto NoOperation() -> Foundation::Bool;
        [[nodiscard]] auto Quit() -> Foundation::Bool;

        auto Disconnect() -> Foundation::Void;

        [[nodiscard]] constexpr auto Connected() const -> Foundation::Bool
        {
            return Connected_;
        }

        [[nodiscard]] constexpr auto LoggedIn() const -> Foundation::Bool
        {
            return LoggedIn_;
        }

        [[nodiscard]] constexpr auto LastStatus() const -> UEFI::StatusCode
        {
            return LastStatus_;
        }

        [[nodiscard]] constexpr auto LastResponse() const -> const FtpResponse&
        {
            return LastResponse_;
        }

    public:
        static constexpr Foundation::Uint16 DefaultPort = 21;
        static constexpr Foundation::Uint16 DefaultDataCapacity = 4096;

    private:
        [[nodiscard]] auto ResolveHost(Library::StringView Host, IPv4Address& Address) -> Foundation::Bool;

        [[nodiscard]] auto SendCommand(Library::StringView Command) -> Foundation::Bool;
        [[nodiscard]] auto SendCommand(Library::StringView Command, Library::StringView Argument) -> Foundation::Bool;

        [[nodiscard]] auto ReadResponse(FtpResponse& Response) -> Foundation::Bool;
        [[nodiscard]] auto ParseResponse(const Library::String& Raw, FtpResponse& Response) -> Foundation::Bool;

        [[nodiscard]] auto EnterPassiveMode(IPv4Address& Address, Foundation::Uint16& Port) -> Foundation::Bool;
        [[nodiscard]] auto ParsePassiveEndpoint(Library::StringView Text, IPv4Address& Address, Foundation::Uint16& Port) -> Foundation::Bool;
        [[nodiscard]] auto OpenPassiveDataConnection(Tcp4Client& Data) -> Foundation::Bool;

        [[nodiscard]] auto ReceiveData(Tcp4Client& DataConnection, Library::Vector<Foundation::Byte>& Data) -> Foundation::Bool;
        [[nodiscard]] auto ReceiveData(Tcp4Client& DataConnection, Library::String& Text) -> Foundation::Bool;
        [[nodiscard]] auto SendData(Tcp4Client& DataConnection, ByteSpan Data) -> Foundation::Bool;

        [[nodiscard]] auto TransferDownload(Library::StringView Command, Library::StringView Path, Library::Vector<Foundation::Byte>& Data) -> Foundation::Bool;
        [[nodiscard]] auto TransferUpload(Library::StringView Command, Library::StringView Path, ByteSpan Data) -> Foundation::Bool;

        [[nodiscard]] auto ParseList(const Library::String& Text, Library::Vector<FtpEntry>& Entries) -> Foundation::Bool;
        [[nodiscard]] auto ParseListLine(Library::StringView Line, FtpEntry& Entry) -> Foundation::Bool;

        [[nodiscard]] auto ExpectPositive(const FtpResponse& Response) -> Foundation::Bool;
        [[nodiscard]] auto ExpectSuccess(const FtpResponse& Response) -> Foundation::Bool;
        [[nodiscard]] auto ExpectTransferStart(const FtpResponse& Response) -> Foundation::Bool;
        [[nodiscard]] auto ExpectTransferFinished(FtpResponse& Response) -> Foundation::Bool;

        auto SetStatus(UEFI::StatusCode Status) -> UEFI::StatusCode;
        auto MoveFrom(FtpClient& Other) -> Foundation::Void;

    private:
        Tcp4Client Control_{};
        Foundation::Bool Connected_{};
        Foundation::Bool LoggedIn_{};
        FtpTransferType TransferType_{ FtpTransferType::Binary };
        FtpResponse LastResponse_{};
        UEFI::StatusCode LastStatus_{ UEFI::StatusCode::Success };
    };
}