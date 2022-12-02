#pragma once

#include<functional>

class ThreadManager
{
private:
	mutex lock;
	vector<thread> threads;
public:
	static void InitTLS();
	static void DestroyTLS();
public:
	void Call(function<void()> callback);
	void Join();
public:
	ThreadManager() { InitTLS(); }
	~ThreadManager() { Join(); }
};