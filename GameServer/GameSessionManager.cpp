#include "pch.h"
#include "GameSessionManager.h"
#include "GameSession.h"

GameSessionManager* GameSessionManager::instance = nullptr;

void GameSessionManager::Add(shared_ptr<GameSession> gameSession)
{
	lock_guard<mutex> guard(lock);
	gameSessions.insert(gameSession);
}

void GameSessionManager::Remove(shared_ptr<GameSession> gameSession)
{
	lock_guard<mutex> guard(lock);
	gameSessions.erase(gameSession);
}

void GameSessionManager::BroadCast(shared_ptr<class SendBuffer> sendBuffer)
{
	lock_guard<mutex> guard(lock);
	for (shared_ptr<GameSession> gameSession : gameSessions)
	{
		gameSession->Send(sendBuffer);

	}
}