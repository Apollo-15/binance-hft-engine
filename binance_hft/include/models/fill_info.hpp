#pragma once
#include <string>
#include <cstdint>

struct FillInfo
{
	double Price;
	double Quantity;
	double Commission;
	std::string CommissionAsset;
	uint64_t TradeId;
};
