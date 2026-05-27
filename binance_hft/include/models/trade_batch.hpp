#pragma once
#include <vector>
#include "trade_event.hpp"

struct TradeBatch
{
	bool IsSold = false;
	std::vector<TradeEvent> Trades;
};
