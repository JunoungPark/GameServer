#include "pch.h"
#include "GameSession.h"
#include <SendBuffer.h>
#include "GameSessionManager.h"

void GameSession::OnConnected()
{
	//추가
	GameSessionManager::Instance()->Add(static_pointer_cast<GameSession>(shared_from_this()));

}
void GameSession::OnDisconnected()
{
	//삭제
	GameSessionManager::Instance()->Remove(static_pointer_cast<GameSession>(shared_from_this()));
}


int32 GameSession::OnRecv(BYTE* buffer, int32 len)
{
	printf("Recv Data : %s\n", (char*)buffer);

	shared_ptr<SendBuffer> sendBuffer = make_shared<SendBuffer>(4096);
	sendBuffer->CopyData(buffer, sizeof(len));

	GameSessionManager::Instance()->BroadCast(sendBuffer);

	return len;
}

void GameSession::OnSend(int32 len)
{
	printf("Send Data Length : %d byte\n", len);
}