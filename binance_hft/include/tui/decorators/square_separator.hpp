#pragma once

#include <vector>
#include <ftxui/dom/elements.hpp>

class SquareSeparator
{
private:
	const int TerminalHeight;
	const std::vector<std::pair<int, double>> PriceLabels;

	SquareSeparator(const int terminalHeight, std::vector<std::pair<int, double>> priceLabels)
		: TerminalHeight(terminalHeight),
		  PriceLabels(std::move(priceLabels))
	{
	}

	friend class DashBoard;

public:
	std::vector<ftxui::Element> BuildRightBorder();
};
