#pragma once

class IocpObj : public enable_shared_from_this<IocpObj>
{
public:
	virtual HANDLE GetHanddle() abstract;
	virtual void Observe(class IocpEvent* iocpEvent, int32 bytes = 0) abstract;
};

class IocpCore
{
private:
	HANDLE iocpHandle;
public:
	IocpCore();
	~IocpCore();

	HANDLE GetHandle();
	bool Register(shared_ptr<IocpObj> iocpObj);
	bool Observe(uint32 time = INFINITY);
};
