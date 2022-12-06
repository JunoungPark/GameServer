#include "pch.h"
#include <SocketHelper.h>
#include <ThreadManager.h>
#include <Service.h>
#include <Session.h>
#include <SendBuffer.h>
char sendData[] = "Hello world";

class ClientSession : public Session
{
public:
	~ClientSession()
	{
		printf("�Ҹ�\n");
	}

	virtual void OnConnected() override
	{
		printf("Connected to Server\n");

		//sendBuffer �Ҵ�
		shared_ptr<SendBuffer> sendBuffer = make_shared<SendBuffer>(4096);
		//sendBuffer�� �ִ� buffer�� sendData�� ����
		sendBuffer->CopyData(sendData, sizeof(sendData));
		//������
		Send(sendBuffer);
	}

	virtual int32 OnRecv(BYTE* buffer, int32 len) override
	{
		printf("OnRecv Length : %d bytes\n", len);

		this_thread::sleep_for(1s);

		//sendBuffer �Ҵ�
		shared_ptr<SendBuffer> sendBuffer = make_shared<SendBuffer>(4096);
		//sendBuffer�� �ִ� buffer�� sendData�� ����
		sendBuffer->CopyData(sendData, sizeof(sendData));
		//������
		Send(sendBuffer);

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


	shared_ptr<ClientService> service = make_shared<ClientService>
		(
			NetworkAddress(L"127.0.0.1", 27015),
			make_shared<IocpCore>(),
			[]() {return make_shared<ClientSession>(); }
	);

	CONDITION_CRASH(service->Start());

	threadManager->Call([=]()
		{
			while (true)
			{
				service->GetIocpCore()->Observe();
			}
		});

	threadManager->Join();

	delete threadManager;
	return 0;
}