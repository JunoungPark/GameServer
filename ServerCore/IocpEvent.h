#pragma once

enum class IO_TYPE : uint8
{
	CONNECT,
	ACCEPT,
	RECV,
	SEND,
	DISCONNECT

};

class IocpEvent : public OVERLAPPED
{
protected:
	IO_TYPE type;
public:
	void Init();
	IO_TYPE GetType();
public:
	IocpEvent(IO_TYPE _type);
	shared_ptr<class IocpObj> owner;
};

class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(IO_TYPE::CONNECT) {}
};
class AcceptEvent : public IocpEvent
{
	
public:

	shared_ptr<class Session> session = nullptr;

	AcceptEvent() : IocpEvent(IO_TYPE::ACCEPT) {}

};

class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(IO_TYPE::RECV){}
};

class SendEvent : public IocpEvent
{
public:
	vector<BYTE> buffer;

	SendEvent():IocpEvent(IO_TYPE::SEND){}
};

class DisconnectEvent : public IocpEvent
{
public:
	DisconnectEvent() :IocpEvent(IO_TYPE::DISCONNECT) { }
};