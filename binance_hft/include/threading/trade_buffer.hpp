#pragma once
#include <deque>
#include <mutex>

#include "models/trade_event.hpp"

struct TradeBuffer
{
private:
	std::deque<TradeEvent> TradeDeque;
	mutable std::mutex Mutex;

public:
	const size_t MaxSize = 25;

	void AddTrade(TradeEvent event);

	const std::deque<TradeEvent>& GetTradeDeque() const
	{
		return TradeDeque;
	}
};
