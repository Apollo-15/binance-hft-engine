#pragma once
#include <string>

#include "binance_config.hpp"

namespace Binance
{
	BinanceConfig ReadConfig(const std::string& path);
}
