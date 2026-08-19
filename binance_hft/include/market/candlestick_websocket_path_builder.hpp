#pragma once

#include "symbol_utilities.hpp"
#include "candlestick_utilities.hpp"

#include <string>
#include <algorithm>
#include <cctype>

inline std::string BuildCandlestickWebSocketPath(const Symbol symbol)
{
	const std::string_view symbolView = SymbolToString(symbol);

	std::string currentSymbol = std::string(symbolView);
	std::ranges::transform(currentSymbol, currentSymbol.begin(), ::tolower);

	std::string path = "/stream?streams=";

	for (int i = 0; i < static_cast<int>(Interval::FinalBorder); i++)
	{
		const std::string newPath = currentSymbol + "@kline_" + std::string(IntervalToString(static_cast<Interval>(i)));
		if (i != 0)
		{
			path += "/";
		}

		path += newPath;
	}

	return path;
}
