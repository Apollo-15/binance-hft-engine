#pragma once

#include <cstdint>

#include "market/candlestick_utilities.hpp"

struct Candle
{
	uint64_t CandlestickStartTime;    // "t"
	double OpenPrice;                // "o"
	double ClosePrice;              // "c"
	double HighPrice;              // "h"
	double LowPrice;              // "l"
	double BaseAssetVolume;      // "v"
	uint64_t NumberOfTrades;    // "n"
	bool IsCandlestickClosed;  // "x"
	uint64_t CandlestickCloseTime;
	Interval Interval;
};
