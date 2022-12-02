#include "pch.h"
#include "ThreadManager.h"

void ThreadManager::InitTLS()
{
	static atomic<uint32> SThreadId = 1;
	LThreadId = SThreadId.fetch_add(1);  // 1추가
}

void ThreadManager::DestroyTLS()
{
	//Todo
}

void ThreadManager::Call(function<void()> callback)
{
	LockGuard guard(lock); // 잡고

	threads.push_back(thread([=]() {

		InitTLS();
		callback();
		DestroyTLS();

		})); // 실행
}// 품

void ThreadManager::Join()
{
	for (thread& t : threads) // vector 돌면서
	{
		if (t.joinable()) // joinable이라면
		{
			t.join(); // 기다려주고
		}
	}

	threads.clear(); // 비우기
}