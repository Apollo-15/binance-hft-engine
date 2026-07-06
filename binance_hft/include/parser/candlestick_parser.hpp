#pragma once

#include <optional>
#include <string>
#include <vector>

#include "models/candle.hpp"

namespace JsonParser
{
	std::optional<Candle> ParseCandlestick(const std::string& json);
	std::optional<std::vector<Candle>> ParseHistoricalCandlestick(const std::string& json, Interval interval);
}
