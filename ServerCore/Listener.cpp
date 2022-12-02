#include "pch.h"
#include "Listener.h"
#include "SocketHelper.h"
#include "IocpEvent.h"
#include "Session.h"
#include "Service.h"
void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
	shared_ptr<Session> session = service->CreateSession();
	acceptEvent->Init();
	acceptEvent->session = session;

	DWORD dwBytes = 0;
	if (!SocketHelper::AcceptEx(socket, session->GetSocket(), session->recvBuffer.WritePos(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytes, reinterpret_cast<LPOVERLAPPED>(acceptEvent)))
    {
        if (WSAGetLastError() != WSA_IO_PENDING)
        {
            RegisterAccept(acceptEvent);
        }
    }
}

void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{

	shared_ptr<Session> session = service->CreateSession();
	if (!SocketHelper::SetUpdateAcceptSocket(session->GetSocket(), socket))
	{
		RegisterAccept(acceptEvent);
		return;
	}

	SOCKADDR_IN sockAddress;
	int32 size = sizeof(sockAddress);
	if (getpeername(session->GetSocket(), reinterpret_cast<SOCKADDR*>(&sockAddress), &size) == SOCKET_ERROR)
	{
		RegisterAccept(acceptEvent);
		return;
	}

	session->SetNetworkAddress(NetworkAddress(sockAddress));

	printf("Client Connected");
	session->ProcessConnect();
	RegisterAccept(acceptEvent);
}

HANDLE Listener::GetHanddle()
{
	return reinterpret_cast<HANDLE>(socket);
}

void Listener::Observe(IocpEvent* iocpEvent, int32 bytes)
{
	if (iocpEvent->GetType() != IO_TYPE::ACCEPT)
	{
		CRASH("Not Accept Type");
	}
	AcceptEvent* acceptEvent = reinterpret_cast<AcceptEvent*>(iocpEvent);
	ProcessAccept(acceptEvent);
}

bool Listener::StartAccept(shared_ptr<ServerService> _service)
{
	socket = SocketHelper::CreateSocket();
	if (socket == INVALID_SOCKET)
	{

		return false;
	}
	if (service->GetIocpCore()->Register(shared_from_this()) == false)
	{
		return false;
	}
	if (SocketHelper::SetReuseAddress(socket, true) == false)
	{
		return false;
	}
	if (SocketHelper::SetLinger(socket, 0, 0) == false)
	{
		return false;
	}
	if (SocketHelper::Bind(socket, service->GetNetworkAddress()) == false)
	{
		return false;
	}
	if (SocketHelper::Listen(socket, 10) == false)
	{
		return false;
	}

	AcceptEvent* acceptEvent = new AcceptEvent;
	acceptEvent->owner = shared_from_this();
	acceptEvents.push_back(acceptEvent);
	RegisterAccept(acceptEvent);


	return true;
}

void Listener::CloseSocket()
{
	SocketHelper::Close(socket);

}

Listener::~Listener()
{
	SocketHelper::Close(socket);
	for (AcceptEvent* acceptEvent : acceptEvents)
	{
		delete acceptEvent;
	}

	acceptEvents.clear();
}