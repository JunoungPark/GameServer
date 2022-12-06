#include "pch.h"
#include "Session.h"
#include "SocketHelper.h"
#include "Service.h"
#include "SendBuffer.h"
Session::Session() : recvBuffer(4096)
{
    socket = SocketHelper::CreateSocket();
}

Session::~Session()
{
    SocketHelper::Close(socket);
}

bool Session::RegisterConnect()
{
	if (IsConnected())
	{
		return false;
	}
	if (GetService()->GetType() != Type::Client)
	{
		return false;
	}
	if (SocketHelper::SetReuseAddress(socket, true) == false)
	{
		return false;
	}
	if (SocketHelper::BindAny(socket, 0) == false)
	{
		return false;
	}

	connectEvent.Init();
	connectEvent.owner = shared_from_this();
	DWORD bytes = 0;
	SOCKADDR_IN sockAddrIn = GetService()->GetNetworkAddress().GetSockAddr();
	if (false == (SocketHelper::lpfnConnectEx(socket, reinterpret_cast<SOCKADDR*>(&sockAddrIn), sizeof(sockAddrIn), nullptr, 0, &bytes, &connectEvent)))
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			connectEvent.owner = nullptr;
			return false;
		}
	}

	return true;
}

void Session::RegisterRecv()
{
	if (IsConnected() == false)
	{
		return;
	}

	recvEvent.Init();

	recvEvent.owner = shared_from_this();

	WSABUF dataBuf;
	dataBuf.buf = reinterpret_cast<char*>(recvBuffer.WritePos());
	dataBuf.len = recvBuffer.FreeSize();

	DWORD recvBytes = 0;
	DWORD flags = 0;

	if (WSARecv(socket, &dataBuf, 1, OUT & recvBytes, OUT & flags, &recvEvent, nullptr) == SOCKET_ERROR)
	{
		int32 errorCode = WSAGetLastError();

		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);

			recvEvent.owner = nullptr;
		}
	}
}

void Session::RegisterSend()
{
	if (IsConnected() == false)
	{
		return;
	}

	sendEvent.Init();
	sendEvent.owner = shared_from_this();

	{
		lock_guard<mutex> guard(lock);

		int32 writeSize = 0;

		while (!sendQueue.empty())
		{
			shared_ptr<SendBuffer> sendBuffer = sendQueue.front();

			writeSize += sendBuffer->WriteSize();

			sendQueue.pop();
			sendEvent.buffers.push_back(sendBuffer);
		}

	}

	vector<WSABUF> wsaBufs;
	wsaBufs.reserve(sendEvent.buffers.size());

	for (shared_ptr<SendBuffer> sendBuffer : sendEvent.buffers)
	{
		WSABUF dataBuf;
		dataBuf.buf = reinterpret_cast<char*>(sendBuffer->Buffer());
		dataBuf.len = static_cast<ULONG>(sendBuffer->WriteSize());

		wsaBufs.push_back(dataBuf);
	}
	DWORD sendBytes = 0;
	if (SOCKET_ERROR == WSASend(socket, wsaBufs.data(), static_cast<DWORD>(wsaBufs.size()), OUT & sendBytes, 0, &sendEvent, NULL))
	{
		int32 errorCode = WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			sendEvent.owner = nullptr;
			sendEvent.buffers.clear();
			sendRegistered.store(false);
		}
	}
}

bool Session::RegisterDisconnect()
{
	disconnectEvent.Init();
	disconnectEvent.owner = shared_from_this();

	if (false == (SocketHelper::DisconnectEx(socket, &disconnectEvent, TF_REUSE_SOCKET, NULL)))
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			disconnectEvent.owner = nullptr;
			return false;
		}
	}

	return true;
}
void Session::ProcessConnect()
{

	connectEvent.owner = nullptr;

	connected.store(true);

	GetService()->AddSession(GetSession());

	OnConnected();

	RegisterRecv();

}

void Session::ProcessSend(int32 bytes)
{
	sendEvent.owner = nullptr;
	sendEvent.buffers.clear();

	if (bytes == 0)
	{
		Disconnect(L"Send data is zero\n");
		return;
	}
	OnSend(bytes);

	lock_guard<mutex> guard(lock);
	if (sendQueue.empty())
	{
		sendRegistered.store(false);
	}
	else
	{
		RegisterSend();
	}
}

void Session::ProcessRecv(int32 bytes)
{
	recvEvent.owner = nullptr;

	if (bytes == 0)
	{
		Disconnect(L"Recv Data 0");
		return;
	}
	if (recvBuffer.OnWrite(bytes) == false)
	{
		Disconnect(L"OnWrite overflow");
		return;
	}
	int32 dataSize = recvBuffer.DataSize();
	
	int32 processLen = OnRecv(recvBuffer.ReadPos(), bytes);
	
	if (processLen < 0)
	{
		Disconnect(L"OnWrite overflow");
		return;
	}

	if (dataSize < processLen)
	{
		Disconnect(L"OnWrite overflow");
		return;
	}

	if (recvBuffer.OnRead(processLen) == false)
	{
		//문제있음 Disconnect
		Disconnect(L"OnWrite overflow");
		return;
	}

	//위치 정리
	recvBuffer.Clean();

	RegisterRecv();
}

void Session::ProcessDisConnect()
{

}

void Session::HandleError(int32 error)
{
	switch (error)
	{
	case WSAECONNRESET:
	case WSAECONNABORTED:
		Disconnect(L"Handle Error");
		break;
	default:
		printf("Handle Error : %d\n", error);
		break;
	}
}

HANDLE Session::GetHanddle()
{
    return reinterpret_cast<HANDLE>(socket);
}

void Session::Observe(IocpEvent* iocpEvent, int32 bytes)
{
	switch (iocpEvent->GetType())
	{
	case IO_TYPE::CONNECT:
		ProcessConnect();
		break;
	case IO_TYPE::RECV:
		ProcessRecv(bytes);
		break;
	case IO_TYPE::SEND:
		ProcessSend(bytes);
		break;
	case IO_TYPE::DISCONNECT:
		ProcessDisConnect();
		break;
	default:
		break;
	}
}

bool Session::Connect()
{
	return RegisterConnect();
}

void Session::Send(shared_ptr<SendBuffer> sendBuffer)
{
	//SendEvent* sendEvent = new SendEvent();
	//
	//sendEvent->owner = shared_from_this();
	//
	//sendEvent->buffers.resize(len);
	//memcpy(sendEvent->buffers.data(), buffer, len);
	{
		lock_guard<mutex> guard(lock);

		sendQueue.push(sendBuffer);
	}
	if (sendRegistered.exchange(true) == false)
	{
		RegisterSend();
	}
}


void Session::Disconnect(const WCHAR* cause)
{
	if (connected.exchange(false) == false)
	{
		return;
	}

	OnDisconnected();

	printf("Disconnect : %S", cause);

	SocketHelper::Close(socket);

	GetService()->RemoveSession(GetSession());

}