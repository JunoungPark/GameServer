#include "pch.h"
#include "RefCounter.h"

RefCounter::RefCounter()
{
    refCount = 1;
}

int32 RefCounter::GetRefCounter()
{
    return int32();
}

int32 RefCounter::AddRef()
{
    return int32();
}

int32 RefCounter::RemoveRef()
{
    int count = --refCount;
    if (count == 0)
    {
        delete this;
    }

    return count;
}
