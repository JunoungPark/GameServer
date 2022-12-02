#pragma once
class NetworkAddress
{
private:

	SOCKADDR_IN service = {};
public:
	NetworkAddress() = default;
	NetworkAddress(SOCKADDR_IN sockAddrIn);
	NetworkAddress(wstring ip, uint16 port);

	SOCKADDR_IN& GetSockAddr();
	wstring GetIp();
	uint16 GetPort();
};