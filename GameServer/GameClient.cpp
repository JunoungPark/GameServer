#include "pch.h"
#include <SocketHelper.h>
#include <ThreadManager.h>
#include <Service.h>
#include <Session.h>

char sendBuffer[] = "Hello world";

class GameSession : public Session
{
public:
	~GameSession()
	{
		printf("¼Ò¸ê\n");
	}

	virtual void OnConnected() override
	{
		printf("Connected to Server\n");
		Send((BYTE*)sendBuffer, sizeof(sendBuffer));
	}

	virtual int32 OnRecv(BYTE* buffer, int32 len)override
	{
		printf("OnRecv Length : %d bytes\n", len);

		this_thread::sleep_for(1s);

		Send((BYTE*)sendBuffer, sizeof(sendBuffer));

		return len;
	}

	virtual void OnSend(int32 len) override
	{
		printf("OnSend Length : %d bytes\n", len);
	}

	virtual void OnDisconnected() override
	{
		printf("Disconnected\n");
	}

};

int main()
{
	ThreadManager* threadManager = new ThreadManager();
	SocketHelper::Init();

	this_thread::sleep_for(1s);

	shared_ptr<ClientService> service = make_shared<ClientService>
		(
			NetworkAddress(L"127.0.0.1", 27015),
			make_shared<IocpCore>(),
			[]() {return make_shared<GameSession>(); });

	CONDITION_CRASH(service->Start());

	threadManager->Call([=]()
		{
			while (true)
			{
				service->GetIocpCore()->Observe();
			}
		}
	); 
	threadManager->Join();

	delete threadManager;
	return 0;
}