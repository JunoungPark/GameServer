#pragma once
#include"pch.h"
class RecvBuffer
{
private:
	const int32 Buffercount = 10;
	int32 bufferSize = 0;
	int32 capacity = 0;
	int32 readPos = 0;
	int32 writePos = 0;
	vector<BYTE> buffer;
public:
	RecvBuffer(int32 _bufferSize);
	~RecvBuffer();

	void Clean();
	bool OnRead(int32 bytes);
	bool OnWrite(int32 bytes);

	BYTE* ReadPos() { return &buffer[readPos]; }
	BYTE* WritePos() { return &buffer[writePos]; }
	int32 DataSize() { return writePos - readPos; }
	int32 FreeSize() { return capacity - writePos; }
};