#include "pch.h"
#include "SocketHelper.h"

LPFN_CONNECTEX SocketHelper::lpfnConnectEx = NULL;
LPFN_DISCONNECTEX SocketHelper::DisconnectEx = NULL;
LPFN_ACCEPTEX SocketHelper::AcceptEx = NULL;
void SocketHelper::Init()
{
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSAData wsaData;

	CONDITION_CRASH(WSAStartup(wVersionRequested, &wsaData) == NO_ERROR);


	SOCKET sock = CreateSocket();

	CONDITION_CRASH(SocketMode(sock, WSAID_CONNECTEX, reinterpret_cast<LPVOID*>(&lpfnConnectEx)));
	CONDITION_CRASH(SocketMode(sock, WSAID_DISCONNECTEX, reinterpret_cast<LPVOID*>(&DisconnectEx)));
	CONDITION_CRASH(SocketMode(sock, WSAID_ACCEPTEX, reinterpret_cast<LPVOID*>(&AcceptEx)));

	Close(sock);
}

void SocketHelper::Clear()
{
	WSACleanup();
}

SOCKET SocketHelper::CreateSocket()
{
	return WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}

bool SocketHelper::SocketMode(SOCKET socket, GUID guid, LPVOID* lpfn)
{
	DWORD bytes = 0;
	bool result = WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), lpfn, sizeof(*lpfn), &bytes, NULL, NULL);
	return result != SOCKET_ERROR;
}

bool SocketHelper::Bind(SOCKET socket, NetworkAddress address)
{
	return bind(socket, (SOCKADDR*)&address.GetSockAddr(), sizeof(SOCKADDR_IN)) != SOCKET_ERROR;
}

bool SocketHelper::BindAny(SOCKET socket, uint16 port)
{
	SOCKADDR_IN service;
	memset(&service, 0, sizeof(service));
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = htonl(INADDR_ANY);
	service.sin_port = htons(port);

	return bind(socket, reinterpret_cast<SOCKADDR*>(&service), sizeof(service)) != SOCKET_ERROR;
}

bool SocketHelper::Listen(SOCKET socket, int32 backlog)
{
	return listen(socket, backlog) != SOCKET_ERROR;
}

void SocketHelper::Close(SOCKET& socket)
{
	if (socket != INVALID_SOCKET)
	{
		closesocket(socket);
		socket = INVALID_SOCKET;
	}
}

bool SocketHelper::SetKeepAlive(SOCKET socket, bool enable)
{
	return setsockopt(socket, SOL_SOCKET, SO_KEEPALIVE, (char*)&enable, sizeof(enable)) != SOCKET_ERROR;
}

bool SocketHelper::SetLinger(SOCKET socket, uint16 onoff, uint16 time)
{
	LINGER linger;
	linger.l_onoff = onoff;
	linger.l_linger = time;

	return setsockopt(socket, SOL_SOCKET, SO_LINGER, (char*)&linger, sizeof(linger)) != SOCKET_ERROR;
}

bool SocketHelper::SetReuseAddress(SOCKET socket, bool enable)
{
	return setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char*)&enable, sizeof(enable)) != SOCKET_ERROR;
}

bool SocketHelper::SetTcpNoDelay(SOCKET socket, bool enable)
{
	return setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (char*)&enable, sizeof(enable)) != SOCKET_ERROR;
}

bool SocketHelper::SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket)
{
	return setsockopt(socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&listenSocket, sizeof(listenSocket) != SOCKET_ERROR);
}
