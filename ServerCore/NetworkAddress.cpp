#include "pch.h"
#include "NetworkAddress.h"

NetworkAddress::NetworkAddress(SOCKADDR_IN sockAddrIn)
{
    service = sockAddrIn;
}

NetworkAddress::NetworkAddress(wstring ip, uint16 port)
{
    memset(&service, 0, sizeof(service));
    service.sin_family = AF_INET;
    IN_ADDR address;
    InetPtonW(AF_INET, ip.c_str(), &address);
    service.sin_addr = address;
    service.sin_port = htons(port);
}

SOCKADDR_IN& NetworkAddress::GetSockAddr()
{
    return service;
}

wstring NetworkAddress::GetIp()
{
    WCHAR buffer[100];
    InetNtopW(AF_INET, &service.sin_addr, buffer, sizeof(buffer) / sizeof(buffer[0]));
    return wstring(buffer);
}

uint16 NetworkAddress::GetPort()
{
    return ntohs(service.sin_port);
}
