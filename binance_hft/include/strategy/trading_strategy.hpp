#pragma once

#include <vector>
#include <optional>
#include "models/trade_batch.hpp"

class TradingStrategy
{
private:
	std::vector<TradeBatch> Batches;

	void BatchCreator(const TradeEvent& event);

public:
	bool ProcessTrades(const TradeEvent& event);
	std::optional<TradeBatch> ShouldSell(double currentPrice);
	TradeBatch& GetBack();
};
