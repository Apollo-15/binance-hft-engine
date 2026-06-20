#pragma once
#include <string>

struct FillInfo
{
	double Price;
	double Quantity;
	double Commission;
	std::string CommissionAsset;
	uint64_t TradeId;
};
