#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"


IocpCore::IocpCore()
{
    iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, (u_long)0, 0);
    if (iocpHandle == NULL)
    {
        CRASH("iocp invalid");
    }
}

IocpCore::~IocpCore()
{
    CloseHandle(iocpHandle);
}

HANDLE IocpCore::GetHandle()
{
    return iocpHandle;
}

bool IocpCore::Register(shared_ptr<IocpObj> iocpObj)
{
    return CreateIoCompletionPort(iocpObj->GetHanddle(), iocpHandle, NULL, NULL);
     
}

bool IocpCore::Observe(uint32 time)
{
    DWORD bytes = 0;
    ULONG_PTR key;
    IocpEvent* iocpEvent = nullptr;

    if (GetQueuedCompletionStatus(iocpHandle, OUT & bytes, OUT & key, reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), time))
    {
        shared_ptr<IocpObj> iocpObj = iocpEvent->owner;
        iocpObj->Observe(iocpEvent, bytes);
    }
    else
    {
        switch (WSAGetLastError())
        {
        case WAIT_TIMEOUT:
            return false;
        default:
            shared_ptr<IocpObj> iocpObj = iocpEvent->owner;
            iocpObj->Observe(iocpEvent, bytes);
            break;
        }
    }
    return true;
}
