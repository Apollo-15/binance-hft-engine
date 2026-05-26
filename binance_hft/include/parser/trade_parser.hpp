#pragma once

#include <optional>

#include "models/trade_event.hpp"

namespace JsonParser
{
	std::optional<TradeEvent> Parse(const std::string& json);
}
