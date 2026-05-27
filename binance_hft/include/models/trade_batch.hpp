#pragma once
#include <vector>
#include "trade_event.hpp"

struct TradeBatch
{
	bool IsSold = false;
	std::vector<TradeEvent> Trades;
	int64_t BatchId = 0;
	double PnL = 0.0;
};
