#pragma once

#include <cstdint>

enum class ConnectionStatus
	: std::uint8_t
{
	Disconnected,
	Connecting,
	Connected,
	Closed,
	Error,
	Reconnecting
};
