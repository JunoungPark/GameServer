#include "pch.h"
#include "ThreadManager.h"

void ThreadManager::InitTLS()
{
	static atomic<uint32> SThreadId = 1;
	LThreadId = SThreadId.fetch_add(1);  // 1�߰�
}

void ThreadManager::DestroyTLS()
{
	//Todo
}

void ThreadManager::Call(function<void()> callback)
{
	LockGuard guard(lock); // ���

	threads.push_back(thread([=]() {

		InitTLS();
		callback();
		DestroyTLS();

		})); // ����
}// ǰ

void ThreadManager::Join()
{
	for (thread& t : threads) // vector ���鼭
	{
		if (t.joinable()) // joinable�̶��
		{
			t.join(); // ��ٷ��ְ�
		}
	}

	threads.clear(); // ����
}