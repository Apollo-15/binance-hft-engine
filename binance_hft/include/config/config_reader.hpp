#pragma once

#include <string>
#include <spdlog/logger.h>

#include "binance_config.hpp"

namespace Binance
{
	BinanceConfig ReadConfig(const std::string& path, std::shared_ptr<spdlog::logger> technicalLogger);
}
