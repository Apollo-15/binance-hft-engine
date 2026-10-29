#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <spdlog/logger.h>

#include "models/candle.hpp"

namespace JsonParser
{
	std::optional<Candle> ParseCandlestick(const std::string& json, const std::shared_ptr<spdlog::logger>& technicalLogger);
	std::optional<std::vector<Candle>> ParseHistoricalCandlestick(const std::string& json, Interval interval, const std::shared_ptr<spdlog::logger>& technicalLogger);
}
