#include "NWebSocketBlueprint.h"
#include "NWebSocketBlueprintModule.h"
#include "WebSocketsModule.h"
#include "IWebSocket.h"

UNWebSocketBlueprint* UNWebSocketBlueprint::CreateWebSocket()
{
	return NewObject<UNWebSocketBlueprint>();
}

UNWebSocketBlueprint::UNWebSocketBlueprint()
	: Super()
	, NativeSocket(nullptr)
{}

void UNWebSocketBlueprint::BeginDestroy()
{
	// �ڶ���GC����ǰ��ȷ���ر����Ӳ�����
	if (NativeSocket.IsValid())
	{
		// ֱ�ӹرգ��⽫����OnClosed�¼�����������CleanupInternal
		NativeSocket->Close();
		NativeSocket.Reset(); // �����ͷ�ָ�룬��ֹBeginDestroy�����¼�����
	}
	Super::BeginDestroy();
}

// �����ڲ��������������������ָ��
void UNWebSocketBlueprint::CleanupInternal()
{
	if (NativeSocket.IsValid())
	{
		NativeSocket->OnConnected().RemoveAll(this);
		NativeSocket->OnConnectionError().RemoveAll(this);
		NativeSocket->OnClosed().RemoveAll(this);
		NativeSocket->OnMessage().RemoveAll(this);
		NativeSocket->OnRawMessage().RemoveAll(this);

		NativeSocket.Reset();
	}

	OnConnectedEvent.Clear();
	OnConnectionErrorEvent.Clear();
	OnCloseEvent.Clear();
	OnMessageEvent.Clear();
	OnRawMessageEvent.Clear();
	OnMessageSentEvent.Clear();

	Headers.Empty();
}

void UNWebSocketBlueprint::Connect(const FString& Url, const FString& Protocol)
{
	if (IsConnected())
	{
		UE_LOG(LogNWebSocketBlueprint, Warning, TEXT("Ignoring Connect(): the socket is already connected to a server."));
		return;
	}

	// ���¼�
	NativeSocket = FWebSocketsModule::Get().CreateWebSocket(Url, Protocol, Headers);

	NativeSocket->OnConnected()      .AddUObject(this, &UNWebSocketBlueprint::OnConnected);
	NativeSocket->OnConnectionError().AddUObject(this, &UNWebSocketBlueprint::OnConnectionError);
	NativeSocket->OnClosed()         .AddUObject(this, &UNWebSocketBlueprint::OnClosed);

	//if (OnMessageEvent.IsBound())
	{
		NativeSocket->OnMessage().AddUObject(this, &UNWebSocketBlueprint::OnMessage);
	}

	//if (OnRawMessageEvent.IsBound())
	{
		NativeSocket->OnRawMessage().AddUObject(this, &UNWebSocketBlueprint::OnRawMessage);
	}
	
	NativeSocket->Connect();
}

void UNWebSocketBlueprint::Close(const int32 Code, const FString& Reason)
{
	if (IsConnected())
	{
		NativeSocket->Close(Code, Reason);
	}
	else
	{
		UE_LOG(LogNWebSocketBlueprint, Warning, TEXT("Close() called but the WebSocket is not connected."));
	}
}

void UNWebSocketBlueprint::SetHeaders(const TMap<FString, FString>& InHeaders)
{
	for (const auto& Header : InHeaders)
	{
		Headers.Add(Header);
	}
}

void UNWebSocketBlueprint::AddHeader(const FString& Header, const FString& Value)
{
	Headers.Add(Header, Value);
}

void UNWebSocketBlueprint::RemoveHeader(const FString& HeaderToRemove)
{
	Headers.Remove(HeaderToRemove);
}

bool UNWebSocketBlueprint::IsConnected() const
{
	return NativeSocket && NativeSocket->IsConnected();
}

void UNWebSocketBlueprint::BranchIsConnected(EBlueprintNWebSocketIsConnectedExecution& Branches)
{
	Branches = IsConnected() ? EBlueprintNWebSocketIsConnectedExecution::Connected : EBlueprintNWebSocketIsConnectedExecution::NotConnected;
}

void UNWebSocketBlueprint::SendMessage(const FString& Message)
{
	if (IsConnected())
	{
		if (Message.Len() <= 0)
		{
			UE_LOG(LogNWebSocketBlueprint, Warning, TEXT("SendMessage() called with an empty message."));
			return;
		}

		NativeSocket->Send(Message);
		OnMessageSent(Message);
	}
	else
	{
		UE_LOG(LogNWebSocketBlueprint, Warning, TEXT("SendMessage() called but the WebSocket is not connected."));
	}
}

void UNWebSocketBlueprint::SendRawMessage(const TArray<uint8> & Message, const bool bIsBinary)
{
	if (IsConnected())
	{
		if (Message.Num() <= 0)
		{
			UE_LOG(LogNWebSocketBlueprint, Warning, TEXT("SendRawMessage() called with an empty message."));
			return;
		}

		NativeSocket->Send(Message.GetData(), sizeof(uint8) * Message.Num(), bIsBinary);
	}
	else
	{
		UE_LOG(LogNWebSocketBlueprint, Warning, TEXT("SendRawMessage() called but the WebSocket is not connected."));
	}
}

void UNWebSocketBlueprint::OnConnected()
{
	OnConnectedEvent.Broadcast();
}

void UNWebSocketBlueprint::OnConnectionError(const FString& Error)
{
	OnConnectionErrorEvent.Broadcast(Error);
	// ���Ӵ���Ҳ��ζ�������������ڽ�����ִ������
	CleanupInternal();
}

void UNWebSocketBlueprint::OnClosed(int32 Status, const FString& Reason, bool bWasClean)
{
	OnCloseEvent.Broadcast(Status, Reason, bWasClean);
	// ���ӹر���ִ����������ʱ��
	CleanupInternal();
}

void UNWebSocketBlueprint::OnMessage(const FString& Message)
{
	OnMessageEvent.Broadcast(Message);
}

void UNWebSocketBlueprint::OnRawMessage(const void* Data, SIZE_T Size, SIZE_T BytesRemaining)
{
	const TArray<uint8> ArrayData(reinterpret_cast<const uint8*>(Data), Size / sizeof(uint8));
	OnRawMessageEvent.Broadcast(ArrayData, static_cast<int64>(BytesRemaining));
}

void UNWebSocketBlueprint::OnMessageSent(const FString& Message)
{
	OnMessageSentEvent.Broadcast(Message);
}
