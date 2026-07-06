#pragma once

class IWebSocketClient
{
private:
public:
	virtual ~IWebSocketClient() = default;
	
	virtual void Reset() = 0;
	virtual void Connect() = 0;
};