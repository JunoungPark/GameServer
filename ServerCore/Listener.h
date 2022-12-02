#pragma once
#include "IocpCore.h"
#include "NetworkAddress.h"
class AcceptEvent;
class ServerService;
class Listener : public IocpObj
{
protected:
	SOCKET socket = INVALID_SOCKET;
	vector<AcceptEvent*> acceptEvents;
	shared_ptr<ServerService> service;
private:
	void RegisterAccept(AcceptEvent* acceptEvent);
	void ProcessAccept(AcceptEvent* acceptEvent);

public:
	virtual HANDLE GetHanddle() override;
	virtual void Observe(class IocpEvent* iocpEvent, int32 bytes = 0) override;

	bool StartAccept(shared_ptr<class ServerService> _service);
	void CloseSocket();

	Listener() = default;
	~Listener();


};

