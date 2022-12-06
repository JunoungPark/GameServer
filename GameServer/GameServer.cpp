#include "pch.h"
#include "SocketHelper.h"
#include "Service.h"
#include "GameSession.h"
#include "ThreadManager.h"

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