#include "pch.h"
#include "RecvBuffer.h"

RecvBuffer::RecvBuffer(int32 _bufferSize)
{
	bufferSize = _bufferSize;

	capacity = bufferSize * Buffercount;

	buffer.resize(capacity);
}

RecvBuffer::~RecvBuffer()
{

}

void RecvBuffer::Clean()
{
	int32 dataSize = DataSize();

	if (dataSize == 0)
	{
		readPos = 0;
		writePos = 0;
	}
	else
	{

		if (FreeSize() < bufferSize)
		{


			memcpy(&buffer[0], &buffer[readPos], dataSize);

			readPos = 0;

			writePos = dataSize;
		}
	}
}

bool RecvBuffer::OnRead(int32 bytes)
{
	if (bytes > DataSize())
	{
		return false;
	}

	readPos += bytes;
	return true;
}

bool RecvBuffer::OnWrite(int32 bytes)
{
	if (bytes > FreeSize())
	{
		return false;
	}

	writePos += bytes;
	return true;
}