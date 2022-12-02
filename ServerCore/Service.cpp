#include "pch.h"
#include "Service.h"
#include "IocpCore.h"
#include "Listener.h"
#include "Session.h"
Service::Service(Type _type, NetworkAddress _address, shared_ptr<IocpCore> _core, SessionFactory _factory)
{
	type = _type;
	address = _address;
	iocpCore = _core;
	sessionFactory = _factory;
}

Service::~Service()
{
}

void Service::EndService()
{
}

shared_ptr<Session> Service::CreateSession()
{
	shared_ptr<Session> session = sessionFactory();

	session->SetService(shared_from_this());
	if (iocpCore->Register(session) == false)
	{
		return nullptr;
	}
	return session;
}

void Service::AddSession(shared_ptr<Session> session)
{
	lock_guard<mutex> guard(lock);
	sessionCount++;
	sessions.insert(session);
}

void Service::RemoveSession(shared_ptr<Session> session)
{
	lock_guard<mutex> guard(lock);
	sessionCount--;
	sessions.erase(session);
}

bool ClientService::Start()
{
	if (sessionFactory == nullptr)
	{
		return false;
	}

	shared_ptr<Session> session = CreateSession();
	if (session->Connect() == false)
	{
		return false;
	}
	return true;
}

bool ServerService::Start()
{
	if (sessionFactory == nullptr)
	{
		return false;
	}

	listener = make_shared<Listener>();
	if (listener == nullptr)
	{
		return false;
	}

	shared_ptr<ServerService> service = static_pointer_cast<ServerService>(shared_from_this());
	if (listener->StartAccept(service) == false)
	{
		return false;
	}
	return true;

}

void ServerService::EndService()
{
	Service::EndService();
}
