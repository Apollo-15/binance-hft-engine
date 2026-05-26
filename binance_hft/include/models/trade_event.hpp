#pragma once

#include <cstdint>
#include <string>

struct TradeEvent
{
	double Price;
	double Quantity;
	int64_t TradeTime;
	int64_t TradeId;
	std::string Symbol;
};