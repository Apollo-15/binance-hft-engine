#pragma once

#include <optional>
#include <spdlog/logger.h>

#include "models/trade_event.hpp"

namespace JsonParser
{
	std::optional<TradeEvent> Parse(const std::string& json, const std::shared_ptr<spdlog::logger>& technicalLogger);
}
