#pragma once
#include <vector>
#include "models/trade_batch.hpp"

class TradingStrategy
{
private:
	std::vector<TradeBatch> Batches;

	void BatchCreator(const TradeEvent& event);

public:
	void ProcessTrades(const TradeEvent& event);
	bool ShouldSell(double currentPrice);
};
