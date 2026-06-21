#pragma once

#include <chrono>
#include <string>

namespace Binance
{
	struct BinanceConfig
	{
		std::string RestHost;
		std::string RestPort;
		std::string WsHost;
		std::string WsPort;
		std::string ApiKey;
		std::string SecretKey;
		std::chrono::seconds Cap;
		std::chrono::seconds BaseDelay;
		std::chrono::seconds JitterMax;
	};
}