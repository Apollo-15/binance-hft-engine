#include "threading/trade_buffer.hpp"

void TradeBuffer::AddTrade(TradeEvent event)
{
	std::scoped_lock<std::mutex> lock(Mutex);

	TradeDeque.push_back(event);

	if (TradeDeque.size() > MaxSize)
	{
		TradeDeque.pop_front();
	}
}
