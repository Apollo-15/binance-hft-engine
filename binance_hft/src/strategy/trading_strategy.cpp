#include <strategy/trading_strategy.hpp>
#include <cmath>

void TradingStrategy::BatchCreator(const TradeEvent& event)
{
	TradeBatch batch;
	batch.Trades.push_back(event);
	Batches.push_back(batch);
}

void TradingStrategy::ProcessTrades(const TradeEvent& event)
{
	if (Batches.empty())
	{
		BatchCreator(event);
	}
	else
	{
		if (std::abs(Batches.back().Trades.back().Price - event.Price) < 1e-8 && 
			std::abs(Batches.back().Trades.back().Quantity - event.Quantity) < 1e-8)
		{


			Batches.back().Trades.push_back(event);
		}
		else
		{
			BatchCreator(event);
		}
	}
}

bool TradingStrategy::ShouldSell(double currentPrice)
{
	for (auto& batch : Batches)
	{
		if (batch.IsSold == false)
		{
			if (currentPrice > batch.Trades.front().Price * 1.002)
			{
				batch.IsSold = true;
				return true;
			}
		}
	}
	return false;
}
