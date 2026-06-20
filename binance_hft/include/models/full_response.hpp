#pragma once
#include <string>
#include <vector>

#include "fill_info.hpp"

struct FullResponse
{
	std::string Symbol;
	uint64_t OrderId;
	int64_t OrderListId;
	std::string ClientOrderId;
	uint64_t TransactionTime;
	double Price;
	double OriginalQuantity;
	double ExecutedQuantity;
	double OriginalQuoteOrderQuantity;
	double CumulativeQuoteQuantity;
	std::string Status;
	std::string TimeInForce;
	std::string Type;
	std::string Side;
	uint64_t WorkingTime;
	std::string SelfTradePreventionMode;
	std::vector<FillInfo> Fills;
};
