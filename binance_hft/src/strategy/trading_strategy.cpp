#include <strategy/trading_strategy.hpp>
#include <cmath>

void TradingStrategy::BatchCreator(const TradeEvent& event)
{
	TradeBatch batch;
	batch.Trades.push_back(event);
	Batches.push_back(batch);
}

bool TradingStrategy::ProcessTrades(const TradeEvent& event)
{
	if (Batches.empty())
	{
		BatchCreator(event);
		return true;
	}
	else
	{
		if (std::abs(Batches.back().Trades.back().Price - event.Price) < 1e-8 && 
			std::abs(Batches.back().Trades.back().Quantity - event.Quantity) < 1e-8)
		{
			Batches.back().Trades.push_back(event);
			return false;
		}
		else
		{
			BatchCreator(event);
			return true;
		}
	}
}

std::optional<TradeBatch> TradingStrategy::ShouldSell(double currentPrice)
{
	for (auto& batch : Batches)
	{
		if (batch.IsSold == false)
		{
			if (currentPrice > batch.Trades.front().Price * 1.002) // change to 1.0001 for test
			{
				batch.IsSold = true;
				return batch;
			}
		}
	}
	return std::nullopt;
}

TradeBatch& TradingStrategy::GetBack()
{
	return Batches.back();
}