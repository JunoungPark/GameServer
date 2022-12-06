#pragma once
class SendBuffer : enable_shared_from_this<SendBuffer>
{
private:
	vector<BYTE> buffer;
	int32 writeSize = 0;
public:
	SendBuffer(int32 bufferSize);
	~SendBuffer();

	BYTE* Buffer() { return buffer.data(); }
	int32 WriteSize() { return writeSize; }
	int32 Capacity() { return static_cast<int32>(buffer.size()); }

	void CopyData(void* data, int32 len);  
};
