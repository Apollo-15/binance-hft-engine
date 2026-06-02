#pragma once

#include <chrono>
#include <string>

namespace Binance
{
	struct BinanceConfig
	{
		std::string Host;
		std::string Port;
		std::chrono::seconds Cap;
		std::chrono::seconds BaseDelay;
		std::chrono::seconds JitterMax;
	};
}