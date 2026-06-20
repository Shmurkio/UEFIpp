#include <UEFIpp/Network/Transport/Tcp4Client.hpp>

#include <UEFIpp/Foundation/Utility.hpp>
#include <UEFIpp/Protocols/Traits.hpp>
#include <UEFIpp/UEFI/Context.hpp>

namespace
{
	using namespace UEFIpp;

	inline constexpr auto EfiOpenProtocolGetProtocol = Foundation::Uint32{ 0x00000002 };
	inline constexpr auto EfiEventNone = Foundation::Uint32{ 0 };
	inline constexpr auto EfiTplCallback = UEFI::Tpl{ 8 };

	auto ToRawAddress(const Network::IPv4Address& Address) -> Protocols::Ipv4RawAddress
	{
		return Protocols::Ipv4RawAddress{ { Address.A(), Address.B(), Address.C(), Address.D() } };
	}
}

namespace UEFIpp::Network
{
	Tcp4Client::~Tcp4Client()
	{
		Close(true);
	}

	Tcp4Client::Tcp4Client(Tcp4Client&& Other) noexcept
	{
		MoveFrom(Other);
	}

	auto Tcp4Client::operator=(Tcp4Client&& Other) noexcept -> Tcp4Client&
	{
		if (this != &Other)
		{
			Close(true);
			MoveFrom(Other);
		}

		return *this;
	}

	auto Tcp4Client::Connect(const IPv4Address& RemoteAddress, Foundation::Uint16 RemotePort) -> Foundation::Bool
	{
		Close(true);

		if (UEFI::IsError(SetStatus(CreateChild())))
		{
			DestroyChild();
			return false;
		}

		if (UEFI::IsError(SetStatus(Configure(RemoteAddress, RemotePort))))
		{
			DestroyChild();
			return false;
		}

		UEFI::Event Event{};

		if (UEFI::IsError(SetStatus(CreateTokenEvent(Event))))
		{
			DestroyChild();
			return false;
		}

		Protocols::Tcp4ConnectionToken Token{};
		Token.CompletionToken.Event = Event;
		Token.CompletionToken.Status = UEFI::StatusCode::NotReady;

		auto Status = Tcp4_->Connect(Tcp4_, &Token);

		if (!UEFI::IsError(Status))
		{
			Status = WaitForToken(Event, Token.CompletionToken.Status);
		}

		UEFI::Context::BootServices().CloseEvent(Event);

		if (UEFI::IsError(SetStatus(Status)))
		{
			DestroyChild();
			return false;
		}

		Connected_ = true;
		OnConnected.Emit();

		return true;
	}

	auto Tcp4Client::Send(ByteSpan Data) -> Foundation::Bool
	{
		if (!Tcp4_ || !Connected_)
		{
			SetStatus(UEFI::StatusCode::NotStarted);
			return false;
		}

		if (Data.Empty())
		{
			SetStatus(UEFI::StatusCode::Success);
			return true;
		}

		if (Data.SizeInBytes() > Foundation::Cast::Auto<Foundation::Size>(~Foundation::Uint32{}))
		{
			SetStatus(UEFI::StatusCode::BadBufferSize);
			return false;
		}

		UEFI::Event Event{};

		if (UEFI::IsError(SetStatus(CreateTokenEvent(Event))))
		{
			return false;
		}

		Protocols::Tcp4TransmitData TxData{};
		TxData.Push = true;
		TxData.Urgent = false;
		TxData.DataLength = Foundation::Cast::Auto<Foundation::Uint32>(Data.SizeInBytes());
		TxData.FragmentCount = 1;
		TxData.FragmentTable[0].FragmentLength = TxData.DataLength;
		TxData.FragmentTable[0].FragmentBuffer = Foundation::Cast::Auto<Foundation::Byte*>(Data.Data());

		Protocols::Tcp4IoToken Token{};
		Token.CompletionToken.Event = Event;
		Token.CompletionToken.Status = UEFI::StatusCode::NotReady;
		Token.Packet.TxData = &TxData;

		auto Status = Tcp4_->Transmit(Tcp4_, &Token);

		if (!UEFI::IsError(Status))
		{
			Status = WaitForToken(Event, Token.CompletionToken.Status);
		}

		UEFI::Context::BootServices().CloseEvent(Event);

		if (UEFI::IsError(SetStatus(Status)))
		{
			return false;
		}

		OnDataSent.Emit(Data);

		return true;
	}

	auto Tcp4Client::Send(Library::StringView Text) -> Foundation::Bool
	{
		const auto Data = ByteSpan
		{
			Foundation::Cast::Auto<const Foundation::Byte*>(Text.Data()),
			Text.Size()
		};

		if (!Send(Data))
		{
			return false;
		}

		OnTextSent.Emit(Text);

		return true;
	}

	auto Tcp4Client::Receive(Library::Vector<Foundation::Byte>& Buffer, Foundation::Size Capacity) -> Foundation::Bool
	{
		if (!Tcp4_ || !Connected_)
		{
			SetStatus(UEFI::StatusCode::NotStarted);
			return false;
		}

		if (Capacity == 0 || Capacity > Foundation::Cast::Auto<Foundation::Size>(~Foundation::Uint32{}))
		{
			SetStatus(UEFI::StatusCode::BadBufferSize);
			return false;
		}

		if (!Buffer.Resize(Capacity))
		{
			SetStatus(UEFI::StatusCode::OutOfResources);
			return false;
		}

		UEFI::Event Event{};

		if (UEFI::IsError(SetStatus(CreateTokenEvent(Event))))
		{
			Buffer.Resize(0);
			return false;
		}

		Protocols::Tcp4ReceiveData RxData{};
		RxData.UrgentFlag = false;
		RxData.DataLength = Foundation::Cast::Auto<Foundation::Uint32>(Capacity);
		RxData.FragmentCount = 1;
		RxData.FragmentTable[0].FragmentLength = RxData.DataLength;
		RxData.FragmentTable[0].FragmentBuffer = Buffer.Data();

		Protocols::Tcp4IoToken Token{};
		Token.CompletionToken.Event = Event;
		Token.CompletionToken.Status = UEFI::StatusCode::NotReady;
		Token.Packet.RxData = &RxData;

		auto Status = Tcp4_->Receive(Tcp4_, &Token);

		if (!UEFI::IsError(Status))
		{
			Status = WaitForToken(Event, Token.CompletionToken.Status);
		}

		UEFI::Context::BootServices().CloseEvent(Event);

		if (UEFI::IsError(SetStatus(Status)))
		{
			Buffer.Resize(0);
			return false;
		}

		Buffer.Resize(RxData.DataLength);
		OnDataReceived.Emit(ByteSpan{ Buffer.Data(), Buffer.Size() });

		return true;
	}

	auto Tcp4Client::Receive(Library::String& Text, Foundation::Size Capacity) -> Foundation::Bool
	{
		Library::Vector<Foundation::Byte> Buffer{};

		if (!Receive(Buffer, Capacity))
		{
			Text.Clear();
			return false;
		}

		Text.Clear();

		for (const auto Byte : Buffer)
		{
			Text.PushBack(Foundation::Cast::Auto<Library::String::ValueType>(Byte));
		}

		OnTextReceived.Emit(Text.View());

		return true;
	}

	auto Tcp4Client::Close(Foundation::Bool Abort) -> Foundation::Void
	{
		const auto WasConnected = Connected_;

		if (Tcp4_ && Connected_)
		{
			UEFI::Event Event{};
			auto Status = CreateTokenEvent(Event);

			if (!UEFI::IsError(Status))
			{
				Protocols::Tcp4CloseToken Token{};
				Token.CompletionToken.Event = Event;
				Token.CompletionToken.Status = UEFI::StatusCode::NotReady;
				Token.AbortOnClose = Abort;

				Status = Tcp4_->Close(Tcp4_, &Token);

				if (!UEFI::IsError(Status))
				{
					Status = WaitForToken(Event, Token.CompletionToken.Status);
				}

				UEFI::Context::BootServices().CloseEvent(Event);
			}

			SetStatus(Status);
		}

		Connected_ = false;
		DestroyChild();

		if (WasConnected)
		{
			OnDisconnected.Emit();
		}
	}

	auto Tcp4Client::CreateChild() -> UEFI::StatusCode
	{
		Protocols::Access Access{ &UEFI::Context::BootServices() };
		auto Handles = Access.LocateHandles<Protocols::Tcp4ServiceBinding>();

		if (!Handles)
		{
			return UEFI::StatusCode::NotFound;
		}

		for (auto Handle : Handles.Value())
		{
			auto Binding = Access.Handle<Protocols::Tcp4ServiceBinding>(Handle);

			if (!Binding)
			{
				continue;
			}

			Tcp4ServiceBinding_ = Binding.Value();
			ControllerHandle_ = Handle;
			ChildHandle_ = nullptr;

			auto Status = Tcp4ServiceBinding_->CreateChild(Tcp4ServiceBinding_, &ChildHandle_);

			if (UEFI::IsError(Status))
			{
				ChildHandle_ = nullptr;
				Tcp4ServiceBinding_ = nullptr;
				ControllerHandle_ = nullptr;
				continue;
			}

			auto Tcp4 = Access.Handle<Protocols::Tcp4>(ChildHandle_);

			if (Tcp4)
			{
				Tcp4_ = Tcp4.Value();
				return UEFI::StatusCode::Success;
			}

			Tcp4ServiceBinding_->DestroyChild(Tcp4ServiceBinding_, ChildHandle_);

			ChildHandle_ = nullptr;
			Tcp4ServiceBinding_ = nullptr;
			ControllerHandle_ = nullptr;
		}

		return UEFI::StatusCode::NotFound;
	}

	auto Tcp4Client::DestroyChild() -> Foundation::Void
	{
		if (Tcp4ServiceBinding_ && ChildHandle_)
		{
			Tcp4ServiceBinding_->DestroyChild(Tcp4ServiceBinding_, ChildHandle_);
		}

		ControllerHandle_ = nullptr;
		ChildHandle_ = nullptr;
		Tcp4ServiceBinding_ = nullptr;
		Tcp4_ = nullptr;
		Connected_ = false;
	}

	auto Tcp4Client::Configure(const IPv4Address& RemoteAddress, Foundation::Uint16 RemotePort) -> UEFI::StatusCode
	{
		if (!Tcp4_)
		{
			return UEFI::StatusCode::NotStarted;
		}

		Protocols::Tcp4Option Options{};
		Options.EnableNagle = true;

		Protocols::Tcp4ConfigData Config{};
		Config.TypeOfService = 0;
		Config.TimeToLive = 64;
		Config.AccessPoint.UseDefaultAddress = true;
		Config.AccessPoint.StationPort = 0;
		Config.AccessPoint.RemoteAddress = ToRawAddress(RemoteAddress);
		Config.AccessPoint.RemotePort = RemotePort;
		Config.AccessPoint.ActiveFlag = true;
		Config.ControlOption = &Options;

		auto Status = Tcp4_->Configure(Tcp4_, &Config);

		if (Status == UEFI::StatusCode::NoMapping)
		{
			auto& BootServices = UEFI::Context::BootServices();

			for (Foundation::Uint32 Attempt{}; Attempt < 500 && Status == UEFI::StatusCode::NoMapping; ++Attempt)
			{
				Tcp4_->Poll(Tcp4_);
				BootServices.Stall(10000);
				Status = Tcp4_->Configure(Tcp4_, &Config);
			}
		}

		return Status;
	}

	auto Tcp4Client::CreateTokenEvent(UEFI::Event& Event) -> UEFI::StatusCode
	{
		Event = nullptr;

		return UEFI::Context::BootServices().CreateEvent(EfiEventNone, EfiTplCallback, nullptr, nullptr, &Event);
	}

	auto Tcp4Client::WaitForToken(UEFI::Event Event, UEFI::StatusCode& CompletionStatus) -> UEFI::StatusCode
	{
		if (!Event)
		{
			return UEFI::StatusCode::InvalidParameter;
		}

		auto& BootServices = UEFI::Context::BootServices();

		while (CompletionStatus == UEFI::StatusCode::NotReady)
		{
			UEFI::Event Events[1]{ Event };
			Foundation::UintN Index{};

			auto Status = BootServices.WaitForEvent(1, Events, &Index);

			if (UEFI::IsError(Status))
			{
				return Status;
			}

			if (Tcp4_)
			{
				Tcp4_->Poll(Tcp4_);
			}
		}

		return CompletionStatus;
	}

	auto Tcp4Client::MoveFrom(Tcp4Client& Other) -> Foundation::Void
	{
		OnConnected = Foundation::Utility::Move(Other.OnConnected);
		OnDisconnected = Foundation::Utility::Move(Other.OnDisconnected);
		OnError = Foundation::Utility::Move(Other.OnError);
		OnDataSent = Foundation::Utility::Move(Other.OnDataSent);
		OnDataReceived = Foundation::Utility::Move(Other.OnDataReceived);
		OnTextSent = Foundation::Utility::Move(Other.OnTextSent);
		OnTextReceived = Foundation::Utility::Move(Other.OnTextReceived);

		ControllerHandle_ = Other.ControllerHandle_;
		ChildHandle_ = Other.ChildHandle_;
		Tcp4ServiceBinding_ = Other.Tcp4ServiceBinding_;
		Tcp4_ = Other.Tcp4_;
		Connected_ = Other.Connected_;
		LastStatus_ = Other.LastStatus_;

		Other.ControllerHandle_ = nullptr;
		Other.ChildHandle_ = nullptr;
		Other.Tcp4ServiceBinding_ = nullptr;
		Other.Tcp4_ = nullptr;
		Other.Connected_ = false;
		Other.LastStatus_ = UEFI::StatusCode::Success;
	}
}