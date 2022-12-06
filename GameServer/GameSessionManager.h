#pragma once

class GameSession;

class GameSessionManager
{
private:
	static GameSessionManager* instance;
public:
	static GameSessionManager* Instance()
	{
		if (instance == nullptr)
		{
			instance = new GameSessionManager;
		}

		return instance;
	}

private:
	mutex lock;
	set<shared_ptr<GameSession>> gameSessions;
public:
	void Add(shared_ptr<GameSession> gameSession);
	void Remove(shared_ptr<GameSession> gameSession);
	void BroadCast(shared_ptr<class SendBuffer> sendBuffer);
};
