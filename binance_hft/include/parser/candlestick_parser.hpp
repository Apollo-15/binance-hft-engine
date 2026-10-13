#pragma once

#include <optional>
#include <string>
#include <vector>
#include <spdlog/logger.h>

#include "models/candle.hpp"

namespace JsonParser
{
	std::optional<Candle> ParseCandlestick(const std::string& json, std::shared_ptr<spdlog::logger> technicalLogger);
	std::optional<std::vector<Candle>> ParseHistoricalCandlestick(const std::string& json, Interval interval, std::shared_ptr<spdlog::logger> technicalLogger);
}
