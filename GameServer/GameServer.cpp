#include "pch.h"
#include "SocketHelper.h"
#include "Service.h"
#include "Session.h"
#include "ThreadManager.h"

class GameSession : public Session
{
public:
	~GameSession()
	{
		printf("¼Ò¸ê\n");
	}

	virtual int32 OnRecv(BYTE* buffer, int32 len) override
	{
		printf("Recv Data Length : %d byte\n", len);
		Send(buffer, len);
		return len;
	}

	virtual void OnSend(int32 len) override
	{
		printf("Send Data Length : %d byte\n", len);
	}

};

int main()
{
	ThreadManager* threadManager = new ThreadManager();
	SocketHelper::Init();

	shared_ptr<ServerService> service = make_shared<ServerService>
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
		});

	threadManager->Join();

	delete(threadManager);


	return 0;
}