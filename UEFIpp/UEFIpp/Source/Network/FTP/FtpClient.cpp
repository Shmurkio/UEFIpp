#include <UEFIpp/Network/FTP/FtpClient.hpp>
#include <UEFIpp/Network/DNS/DnsClient.hpp>

namespace
{
    using namespace UEFIpp;

    auto IsDigit(Foundation::Char Ch) -> Foundation::Bool
    {
        return Ch >= '0' && Ch <= '9';
    }

    auto ReadCode(Library::StringView Text, Foundation::Uint32& Code) -> Foundation::Bool
    {
        if (Text.Size() < 3 || !IsDigit(Text[0]) || !IsDigit(Text[1]) || !IsDigit(Text[2]))
        {
            return false;
        }

        Code = Foundation::Cast::Auto<Foundation::Uint32>((Text[0] - '0') * 100 + (Text[1] - '0') * 10 + (Text[2] - '0'));
        return true;
    }

    auto AppendDecimal(Library::String& Text, Foundation::Uint64 Value) -> Foundation::Void
    {
        Foundation::Char Buffer[32]{};
        Foundation::Size Index = sizeof(Buffer);

        do
        {
            Buffer[--Index] = Foundation::Cast::Auto<Foundation::Char>('0' + (Value % 10));
            Value /= 10;
        } while (Value);

        while (Index < sizeof(Buffer))
        {
            Text += Buffer[Index++];
        }
    }

    auto ParseDecimal(Library::StringView Text, Foundation::Size& Offset, Foundation::Uint32& Value) -> Foundation::Bool
    {
        Value = 0;

        if (Offset >= Text.Size() || !IsDigit(Text[Offset]))
        {
            return false;
        }

        while (Offset < Text.Size() && IsDigit(Text[Offset]))
        {
            Value = Value * 10 + Foundation::Cast::Auto<Foundation::Uint32>(Text[Offset] - '0');
            ++Offset;
        }

        return true;
    }

    auto TrimLeft(Library::StringView Text) -> Library::StringView
    {
        Foundation::Size Offset{};

        while (Offset < Text.Size() && (Text[Offset] == ' ' || Text[Offset] == '\t'))
        {
            ++Offset;
        }

        return Text.Substr(Offset);
    }

    auto TrimRight(Library::StringView Text) -> Library::StringView
    {
        auto Size = Text.Size();

        while (Size && (Text[Size - 1] == ' ' || Text[Size - 1] == '\t' || Text[Size - 1] == '\r' || Text[Size - 1] == '\n'))
        {
            --Size;
        }

        return Text.Substr(0, Size);
    }

    auto Trim(Library::StringView Text) -> Library::StringView
    {
        return TrimRight(TrimLeft(Text));
    }

    auto CompleteFtpResponse(Library::StringView Raw) -> Foundation::Bool
    {
        if (Raw.Size() < 5)
        {
            return false;
        }

        Foundation::Uint32 Code{};

        if (!ReadCode(Raw, Code))
        {
            return false;
        }

        if (Raw.Size() > 3 && Raw[3] == ' ')
        {
            return Raw.Find("\r\n") != Library::StringView::NotFound;
        }

        if (Raw.Size() <= 3 || Raw[3] != '-')
        {
            return false;
        }

        Foundation::Size Offset{};

        while (Offset < Raw.Size())
        {
            const auto LineEnd = Raw.Find("\r\n", Offset);

            if (LineEnd == Library::StringView::NotFound)
            {
                return false;
            }

            const auto Line = Raw.Substr(Offset, LineEnd - Offset);

            Foundation::Uint32 LineCode{};

            if (Line.Size() >= 4 && ReadCode(Line, LineCode) && LineCode == Code && Line[3] == ' ')
            {
                return true;
            }

            Offset = LineEnd + 2;
        }

        return false;
    }
}

namespace UEFIpp::Network
{
    FtpClient::~FtpClient()
    {
        Disconnect();
    }

    FtpClient::FtpClient(FtpClient&& Other) noexcept
    {
        MoveFrom(Other);
    }

    auto FtpClient::operator=(FtpClient&& Other) noexcept -> FtpClient&
    {
        if (this != &Other)
        {
            Disconnect();
            MoveFrom(Other);
        }

        return *this;
    }

    auto FtpClient::Connect(const IPv4Address& Address, Foundation::Uint16 Port) -> Foundation::Bool
    {
        Disconnect();

        if (!Control_.Connect(Address, Port))
        {
            SetStatus(Control_.LastStatus());
            return false;
        }

        Connected_ = true;
        OnConnected();

        FtpResponse Response{};

        if (!ReadResponse(Response))
        {
            Disconnect();
            return false;
        }

        if (!ExpectSuccess(Response))
        {
            Disconnect();
            return false;
        }

        return true;
    }

    auto FtpClient::Connect(Library::StringView Host, Foundation::Uint16 Port) -> Foundation::Bool
    {
        IPv4Address Address{};

        if (!ResolveHost(Host, Address))
        {
            return false;
        }

        return Connect(Address, Port);
    }

    auto FtpClient::Login(Library::StringView Username, Library::StringView Password) -> Foundation::Bool
    {
        if (!Connected_)
        {
            SetStatus(UEFI::StatusCode::NotStarted);
            return false;
        }

        if (!SendCommand("USER", Username))
        {
            return false;
        }

        FtpResponse Response{};

        if (!ReadResponse(Response))
        {
            return false;
        }

        if (Response.LoggedIn())
        {
            LoggedIn_ = true;
            OnLoggedIn();
            return true;
        }

        if (!Response.NeedPassword())
        {
            SetStatus(UEFI::StatusCode::AccessDenied);
            return false;
        }

        if (!SendCommand("PASS", Password))
        {
            return false;
        }

        if (!ReadResponse(Response))
        {
            return false;
        }

        if (!Response.LoggedIn())
        {
            SetStatus(UEFI::StatusCode::AccessDenied);
            return false;
        }

        LoggedIn_ = true;
        OnLoggedIn();

        return true;
    }

    auto FtpClient::LoginAnonymous() -> Foundation::Bool
    {
        return Login("anonymous", "anonymous@");
    }

    auto FtpClient::SetTransferType(FtpTransferType Type) -> Foundation::Bool
    {
        const auto Argument = Type == FtpTransferType::Binary ? Library::StringView{ "I" } : Library::StringView{ "A" };

        if (!SendCommand("TYPE", Argument))
        {
            return false;
        }

        FtpResponse Response{};

        if (!ReadResponse(Response) || !ExpectSuccess(Response))
        {
            return false;
        }

        TransferType_ = Type;
        return true;
    }

    auto FtpClient::SetBinary() -> Foundation::Bool
    {
        return SetTransferType(FtpTransferType::Binary);
    }

    auto FtpClient::SetAscii() -> Foundation::Bool
    {
        return SetTransferType(FtpTransferType::Ascii);
    }

    auto FtpClient::PrintWorkingDirectory(Library::String& Path) -> Foundation::Bool
    {
        Path.Clear();

        if (!SendCommand("PWD"))
        {
            return false;
        }

        FtpResponse Response{};

        if (!ReadResponse(Response) || !ExpectSuccess(Response))
        {
            return false;
        }

        const auto Message = Response.Message.View();
        const auto Begin = Message.Find("\"");

        if (Begin == Library::StringView::NotFound)
        {
            Path = Response.Message;
            return true;
        }

        const auto End = Message.Find("\"", Begin + 1);

        if (End == Library::StringView::NotFound)
        {
            Path = Message.Substr(Begin + 1);
            return true;
        }

        Path = Message.Substr(Begin + 1, End - Begin - 1);
        return true;
    }

    auto FtpClient::ChangeDirectory(Library::StringView Path) -> Foundation::Bool
    {
        if (!SendCommand("CWD", Path))
        {
            return false;
        }

        FtpResponse Response{};
        return ReadResponse(Response) && ExpectSuccess(Response);
    }

    auto FtpClient::MakeDirectory(Library::StringView Path) -> Foundation::Bool
    {
        if (!SendCommand("MKD", Path))
        {
            return false;
        }

        FtpResponse Response{};
        return ReadResponse(Response) && ExpectSuccess(Response);
    }

    auto FtpClient::RemoveDirectory(Library::StringView Path) -> Foundation::Bool
    {
        if (!SendCommand("RMD", Path))
        {
            return false;
        }

        FtpResponse Response{};
        return ReadResponse(Response) && ExpectSuccess(Response);
    }

    auto FtpClient::DeleteFile(Library::StringView Path) -> Foundation::Bool
    {
        if (!SendCommand("DELE", Path))
        {
            return false;
        }

        FtpResponse Response{};
        return ReadResponse(Response) && ExpectSuccess(Response);
    }

    auto FtpClient::List(Library::Vector<FtpEntry>& Entries, Library::StringView Path) -> Foundation::Bool
    {
        Entries.Clear();

        Library::String Text{};

        if (!ListRaw(Text, Path))
        {
            return false;
        }

        return ParseList(Text, Entries);
    }

    auto FtpClient::ListRaw(Library::String& Text, Library::StringView Path) -> Foundation::Bool
    {
        Library::Vector<Foundation::Byte> Data{};

        if (!TransferDownload("LIST", Path, Data))
        {
            Text.Clear();
            return false;
        }

        Text.Clear();

        for (const auto Byte : Data)
        {
            Text.PushBack(Foundation::Cast::Auto<Library::String::ValueType>(Byte));
        }

        return true;
    }

    auto FtpClient::Download(Library::StringView Path, Library::Vector<Foundation::Byte>& Data) -> Foundation::Bool
    {
        return TransferDownload("RETR", Path, Data);
    }

    auto FtpClient::DownloadText(Library::StringView Path, Library::String& Text) -> Foundation::Bool
    {
        Library::Vector<Foundation::Byte> Data{};

        if (!Download(Path, Data))
        {
            Text.Clear();
            return false;
        }

        Text.Clear();

        for (const auto Byte : Data)
        {
            Text.PushBack(Foundation::Cast::Auto<Library::String::ValueType>(Byte));
        }

        return true;
    }

    auto FtpClient::Upload(Library::StringView Path, ByteSpan Data) -> Foundation::Bool
    {
        return TransferUpload("STOR", Path, Data);
    }

    auto FtpClient::UploadText(Library::StringView Path, Library::StringView Text) -> Foundation::Bool
    {
        return Upload(Path, ByteSpan{ Foundation::Cast::Auto<const Foundation::Byte*>(Text.Data()), Text.Size() });
    }

    auto FtpClient::NoOperation() -> Foundation::Bool
    {
        if (!SendCommand("NOOP"))
        {
            return false;
        }

        FtpResponse Response{};
        return ReadResponse(Response) && ExpectSuccess(Response);
    }

    auto FtpClient::Quit() -> Foundation::Bool
    {
        if (!Connected_)
        {
            SetStatus(UEFI::StatusCode::Success);
            return true;
        }

        if (!SendCommand("QUIT"))
        {
            Disconnect();
            return false;
        }

        FtpResponse Response{};
        const auto Result = ReadResponse(Response) && ExpectSuccess(Response);

        Disconnect();
        return Result;
    }

    auto FtpClient::Disconnect() -> Foundation::Void
    {
        const auto WasConnected = Connected_;

        Control_.Close(true);
        Connected_ = false;
        LoggedIn_ = false;

        if (WasConnected)
        {
            OnDisconnected();
        }
    }

    auto FtpClient::ResolveHost(Library::StringView Host, IPv4Address& Address) -> Foundation::Bool
    {
        OnResolvingHost(Host);

        DnsClient Dns{};

        if (!Dns.Resolve(Host, Address))
        {
            SetStatus(Dns.LastStatus());
            return false;
        }

        OnHostResolved(Address);
        return true;
    }

    auto FtpClient::SendCommand(Library::StringView Command) -> Foundation::Bool
    {
        if (!Connected_)
        {
            SetStatus(UEFI::StatusCode::NotStarted);
            return false;
        }

        Library::String Text{};
        Text += Command;
        Text += "\r\n";

        if (!Control_.Send(Text.View()))
        {
            SetStatus(Control_.LastStatus());
            return false;
        }

        OnCommandSent(Text.View());
        return true;
    }

    auto FtpClient::SendCommand(Library::StringView Command, Library::StringView Argument) -> Foundation::Bool
    {
        if (Argument.Empty())
        {
            return SendCommand(Command);
        }

        if (!Connected_)
        {
            SetStatus(UEFI::StatusCode::NotStarted);
            return false;
        }

        Library::String Text{};
        Text += Command;
        Text += " ";
        Text += Argument;
        Text += "\r\n";

        if (!Control_.Send(Text.View()))
        {
            SetStatus(Control_.LastStatus());
            return false;
        }

        OnCommandSent(Text.View());

        return true;
    }

    auto FtpClient::ReadResponse(FtpResponse& Response) -> Foundation::Bool
    {
        Response = {};

        if (!Connected_)
        {
            SetStatus(UEFI::StatusCode::NotStarted);
            return false;
        }

        Library::String Raw{};

        for (Foundation::Uint32 Attempt{}; Attempt < 8; ++Attempt)
        {
            Library::String Chunk{};

            if (!Control_.Receive(Chunk, DefaultDataCapacity))
            {
                SetStatus(Control_.LastStatus());
                return false;
            }

            Raw += Chunk;

            if (CompleteFtpResponse(Raw.View()))
            {
                break;
            }
        }

        if (!ParseResponse(Raw, Response))
        {
            return false;
        }

        LastResponse_ = Response;
        OnResponseReceived(Response);

        return true;
    }

    auto FtpClient::ParseResponse(const Library::String& Raw, FtpResponse& Response) -> Foundation::Bool
    {
        Response = {};
        Response.Raw = Raw;

        if (!CompleteFtpResponse(Raw.View()))
        {
            SetStatus(UEFI::StatusCode::InvalidParameter);
            return false;
        }

        Foundation::Uint32 Code{};

        if (!ReadCode(Raw.View(), Code))
        {
            SetStatus(UEFI::StatusCode::InvalidParameter);
            return false;
        }

        Response.Code = Code;

        Foundation::Size Offset{};

        while (Offset < Raw.Size())
        {
            const auto LineEnd = Raw.View().Find("\r\n", Offset);

            if (LineEnd == Library::StringView::NotFound)
            {
                break;
            }

            auto Line = Raw.View().Substr(Offset, LineEnd - Offset);
            Response.Lines.PushBack(Line);

            if (Line.Size() > 4)
            {
                if (!Response.Message.Empty())
                {
                    Response.Message += "\n";
                }

                Response.Message += Line.Substr(4);
            }

            Offset = LineEnd + 2;
        }

        SetStatus(UEFI::StatusCode::Success);
        return true;
    }

    auto FtpClient::EnterPassiveMode(IPv4Address& Address, Foundation::Uint16& Port) -> Foundation::Bool
    {
        Address = {};
        Port = 0;

        if (!SendCommand("PASV"))
        {
            return false;
        }

        FtpResponse Response{};

        if (!ReadResponse(Response) || !ExpectSuccess(Response))
        {
            return false;
        }

        if (!ParsePassiveEndpoint(Response.Message.View(), Address, Port))
        {
            SetStatus(UEFI::StatusCode::InvalidParameter);
            return false;
        }

        OnPassiveModeEntered(Address, Port);

        return true;
    }

    auto FtpClient::ParsePassiveEndpoint(Library::StringView Text, IPv4Address& Address, Foundation::Uint16& Port) -> Foundation::Bool
    {
        const auto Begin = Text.Find("(");
        const auto End = Text.Find(")");

        if (Begin == Library::StringView::NotFound || End == Library::StringView::NotFound || End <= Begin)
        {
            return false;
        }

        auto Data = Text.Substr(Begin + 1, End - Begin - 1);
        Foundation::Size Offset{};
        Foundation::Uint32 Values[6]{};

        for (Foundation::Size Index{}; Index < 6; ++Index)
        {
            if (!ParseDecimal(Data, Offset, Values[Index]) || Values[Index] > 255)
            {
                return false;
            }

            if (Index < 5)
            {
                if (Offset >= Data.Size() || Data[Offset] != ',')
                {
                    return false;
                }

                ++Offset;
            }
        }

        Address = IPv4Address
        {
            Foundation::Cast::Auto<Foundation::Byte>(Values[0]),
            Foundation::Cast::Auto<Foundation::Byte>(Values[1]),
            Foundation::Cast::Auto<Foundation::Byte>(Values[2]),
            Foundation::Cast::Auto<Foundation::Byte>(Values[3])
        };

        Port = Foundation::Cast::Auto<Foundation::Uint16>((Values[4] << 8) | Values[5]);
        return true;
    }

    auto FtpClient::OpenPassiveDataConnection(Tcp4Client& Data) -> Foundation::Bool
    {
        IPv4Address Address{};
        Foundation::Uint16 Port{};

        if (!EnterPassiveMode(Address, Port))
        {
            return false;
        }

        if (!Data.Connect(Address, Port))
        {
            SetStatus(Data.LastStatus());
            return false;
        }

        return true;
    }

    auto FtpClient::ReceiveData(Tcp4Client& DataConnection, Library::Vector<Foundation::Byte>& Data) -> Foundation::Bool
    {
        Data.Clear();

        for (;;)
        {
            Library::Vector<Foundation::Byte> Chunk{};

            if (!DataConnection.Receive(Chunk, DefaultDataCapacity))
            {
                break;
            }

            if (Chunk.Empty())
            {
                break;
            }

            for (const auto Byte : Chunk)
            {
                Data.PushBack(Byte);
            }

            OnDataReceived(ByteSpan{ Chunk.Data(), Chunk.Size() });
        }

        SetStatus(UEFI::StatusCode::Success);
        return true;
    }

    auto FtpClient::ReceiveData(Tcp4Client& DataConnection, Library::String& Text) -> Foundation::Bool
    {
        Library::Vector<Foundation::Byte> Data{};

        if (!ReceiveData(DataConnection, Data))
        {
            Text.Clear();
            return false;
        }

        Text.Clear();

        for (const auto Byte : Data)
        {
            Text.PushBack(Foundation::Cast::Auto<Library::String::ValueType>(Byte));
        }

        return true;
    }

    auto FtpClient::SendData(Tcp4Client& DataConnection, ByteSpan Data) -> Foundation::Bool
    {
        if (!DataConnection.Send(Data))
        {
            SetStatus(DataConnection.LastStatus());
            return false;
        }

        OnDataSent(Data);
        return true;
    }

    auto FtpClient::TransferDownload(Library::StringView Command, Library::StringView Path, Library::Vector<Foundation::Byte>& Data) -> Foundation::Bool
    {
        Data.Clear();
        Tcp4Client DataConnection{};

        if (!OpenPassiveDataConnection(DataConnection))
        {
            return false;
        }

        OnTransferStarted();

        if (!SendCommand(Command, Path))
        {
            DataConnection.Close(true);
            return false;
        }

        FtpResponse Response{};

        if (!ReadResponse(Response) || !ExpectTransferStart(Response))
        {
            DataConnection.Close(true);
            return false;
        }

        if (!ReceiveData(DataConnection, Data))
        {
            DataConnection.Close(true);
            return false;
        }

        DataConnection.Close();

        if (!ExpectTransferFinished(Response))
        {
            return false;
        }

        OnTransferFinished();
        return true;
    }

    auto FtpClient::TransferUpload(Library::StringView Command, Library::StringView Path, ByteSpan Data) -> Foundation::Bool
    {
        Tcp4Client DataConnection{};

        if (!OpenPassiveDataConnection(DataConnection))
        {
            return false;
        }

        OnTransferStarted();

        if (!SendCommand(Command, Path))
        {
            DataConnection.Close(true);
            return false;
        }

        FtpResponse Response{};

        if (!ReadResponse(Response) || !ExpectTransferStart(Response))
        {
            DataConnection.Close(true);
            return false;
        }

        if (!SendData(DataConnection, Data))
        {
            DataConnection.Close(true);
            return false;
        }

        DataConnection.Close();

        if (!ExpectTransferFinished(Response))
        {
            return false;
        }

        OnTransferFinished();
        return true;
    }

    auto FtpClient::ParseList(const Library::String& Text, Library::Vector<FtpEntry>& Entries) -> Foundation::Bool
    {
        Entries.Clear();

        Foundation::Size Offset{};

        while (Offset < Text.Size())
        {
            auto LineEnd = Text.View().Find("\n", Offset);
            const auto RawLine = LineEnd == Library::StringView::NotFound ? Text.View().Substr(Offset) : Text.View().Substr(Offset, LineEnd - Offset);
            const auto Line = Trim(RawLine);

            if (!Line.Empty())
            {
                FtpEntry Entry{};

                if (ParseListLine(Line, Entry) && !Entry.Empty())
                {
                    Entries.PushBack(Foundation::Utility::Move(Entry));
                    OnEntryReceived(Entries[Entries.Size() - 1]);
                }
            }

            if (LineEnd == Library::StringView::NotFound)
            {
                break;
            }

            Offset = LineEnd + 1;
        }

        SetStatus(UEFI::StatusCode::Success);
        return true;
    }

    auto FtpClient::ParseListLine(Library::StringView Line, FtpEntry& Entry) -> Foundation::Bool
    {
        Entry = {};
        Entry.Raw = Line;

        if (Line.Empty())
        {
            return false;
        }

        if (Line[0] == 'd')
        {
            Entry.Type = FtpEntryType::Directory;
        }
        else if (Line[0] == '-')
        {
            Entry.Type = FtpEntryType::File;
        }
        else if (Line[0] == 'l')
        {
            Entry.Type = FtpEntryType::SymbolicLink;
        }
        else
        {
            Entry.Type = FtpEntryType::Unknown;
        }

        Foundation::Size Field{};
        Foundation::Size Offset{};
        Foundation::Size NameStart{};

        while (Offset < Line.Size())
        {
            while (Offset < Line.Size() && Line[Offset] == ' ')
            {
                ++Offset;
            }

            const auto Start = Offset;

            while (Offset < Line.Size() && Line[Offset] != ' ')
            {
                ++Offset;
            }

            if (Start == Offset)
            {
                break;
            }

            ++Field;

            if (Field == 5)
            {
                Foundation::Uint64 Size{};

                for (Foundation::Size Index = Start; Index < Offset; ++Index)
                {
                    if (!IsDigit(Line[Index]))
                    {
                        break;
                    }

                    Size = Size * 10 + Foundation::Cast::Auto<Foundation::Uint64>(Line[Index] - '0');
                }

                Entry.Size = Size;
            }
            else if (Field == 9)
            {
                NameStart = Start;
                break;
            }
        }

        if (NameStart < Line.Size())
        {
            auto Name = Trim(Line.Substr(NameStart));
            const auto Link = Name.Find(" -> ");

            if (Link != Library::StringView::NotFound)
            {
                Name = Name.Substr(0, Link);
            }

            Entry.Name = Name;
        }
        else
        {
            Entry.Name = Line;
        }

        return true;
    }

    auto FtpClient::ExpectPositive(const FtpResponse& Response) -> Foundation::Bool
    {
        if (!Response.Positive())
        {
            SetStatus(UEFI::StatusCode::DeviceError);
            return false;
        }

        return true;
    }

    auto FtpClient::ExpectSuccess(const FtpResponse& Response) -> Foundation::Bool
    {
        if (!Response.Success())
        {
            SetStatus(UEFI::StatusCode::DeviceError);
            return false;
        }

        return true;
    }

    auto FtpClient::ExpectTransferStart(const FtpResponse& Response) -> Foundation::Bool
    {
        if (!Response.OpeningDataConnection())
        {
            SetStatus(UEFI::StatusCode::DeviceError);
            return false;
        }

        return true;
    }

    auto FtpClient::ExpectTransferFinished(FtpResponse& Response) -> Foundation::Bool
    {
        if (Response.TransferComplete())
        {
            return true;
        }

        if (!ReadResponse(Response))
        {
            return false;
        }

        if (!Response.TransferComplete())
        {
            SetStatus(UEFI::StatusCode::DeviceError);
            return false;
        }

        return true;
    }

    auto FtpClient::SetStatus(UEFI::StatusCode Status) -> UEFI::StatusCode
    {
        LastStatus_ = Status;

        if (UEFI::IsError(Status))
        {
            OnError(Status);
        }

        return Status;
    }

    auto FtpClient::MoveFrom(FtpClient& Other) -> Foundation::Void
    {
        OnConnected = Foundation::Utility::Move(Other.OnConnected);
        OnDisconnected = Foundation::Utility::Move(Other.OnDisconnected);
        OnLoggedIn = Foundation::Utility::Move(Other.OnLoggedIn);
        OnTransferStarted = Foundation::Utility::Move(Other.OnTransferStarted);
        OnTransferFinished = Foundation::Utility::Move(Other.OnTransferFinished);
        OnError = Foundation::Utility::Move(Other.OnError);
        OnResolvingHost = Foundation::Utility::Move(Other.OnResolvingHost);
        OnHostResolved = Foundation::Utility::Move(Other.OnHostResolved);
        OnCommandSent = Foundation::Utility::Move(Other.OnCommandSent);
        OnResponseReceived = Foundation::Utility::Move(Other.OnResponseReceived);
        OnPassiveModeEntered = Foundation::Utility::Move(Other.OnPassiveModeEntered);
        OnDataReceived = Foundation::Utility::Move(Other.OnDataReceived);
        OnDataSent = Foundation::Utility::Move(Other.OnDataSent);
        OnEntryReceived = Foundation::Utility::Move(Other.OnEntryReceived);

        Control_ = Foundation::Utility::Move(Other.Control_);
        Connected_ = Other.Connected_;
        LoggedIn_ = Other.LoggedIn_;
        TransferType_ = Other.TransferType_;
        LastResponse_ = Foundation::Utility::Move(Other.LastResponse_);
        LastStatus_ = Other.LastStatus_;

        Other.Connected_ = false;
        Other.LoggedIn_ = false;
        Other.TransferType_ = FtpTransferType::Binary;
        Other.LastResponse_ = {};
        Other.LastStatus_ = UEFI::StatusCode::Success;
    }
}