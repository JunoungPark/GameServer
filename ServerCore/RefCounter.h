#pragma once
class RefCounter
{
protected:
	atomic<int32> refCount;
public:
	RefCounter();
	virtual ~RefCounter() {};

	int32 GetRefCounter();
	int32 AddRef();
	int32 RemoveRef();
};

