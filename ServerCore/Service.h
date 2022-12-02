#pragma once
#include <functional>
#include "NetworkAddress.h"
enum class Type : uint8
{
	Server,
	Client
};

class IocpCore;
class Session;

using SessionFactory = function<shared_ptr<Session>(void)>;

class Service : public enable_shared_from_this<Service>
{
protected:
	mutex lock;
	Type type;
	NetworkAddress address = {};
	shared_ptr<IocpCore> iocpCore;
	SessionFactory sessionFactory;
	set<shared_ptr<class Session>> sessions;
	int32 sessionCount = 0;
public:
	Service(Type _type, NetworkAddress _address, shared_ptr<IocpCore> _core, SessionFactory _factory);
	virtual ~Service();

	virtual bool Start() abstract;
	virtual void EndService();

	Type GetType() { return type; }
	NetworkAddress GetNetworkAddress() { return address; }
	shared_ptr<IocpCore>& GetIocpCore() { return iocpCore; }
	void SetSessionFactory(SessionFactory func) { sessionFactory = func; }
	shared_ptr<Session> CreateSession();
	void AddSession(shared_ptr<Session> session);
	void RemoveSession(shared_ptr<Session> session);
	int32 GetSessionCount() { return sessionCount; }
};

class ClientService : public Service
{
public:
	ClientService(NetworkAddress _address, shared_ptr<IocpCore> _core, SessionFactory _factory) :Service::Service(Type::Client, _address, _core, _factory) {}
	

	virtual bool Start() override;
};

class ServerService : public Service
{
private:
	shared_ptr<class Listener> listener = nullptr;


public:
	ServerService(NetworkAddress _address, shared_ptr<IocpCore> _core, SessionFactory _factory) : Service::Service(Type::Server, _address, _core, _factory){}


	virtual bool Start() override;
	virtual void EndService();
};