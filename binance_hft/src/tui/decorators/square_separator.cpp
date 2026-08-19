#include "tui/decorators/square_separator.hpp"

std::vector<ftxui::Element> SquareSeparator::BuildRightPriceBorder()
{
	std::vector<ftxui::Element> elements;

	for (int verticalRow = 0; verticalRow <= TerminalHeight; verticalRow++)
	{
		bool foundLabel = false;

		for (const std::pair<int, double> priceLabel : PriceLabels)
		{	
			if (priceLabel.first == verticalRow)
			{
				elements.push_back(ftxui::text(reinterpret_cast<const char*>(u8"┣━")));
				foundLabel = true;
				break;
			}
		}

		if (!foundLabel)
		{
			elements.push_back(ftxui::text(reinterpret_cast<const char*>(u8"┃ ")));
		}
	}

	return elements;
}

std::vector<ftxui::Element> SquareSeparator::BuildLeftVolumeBorder()
{
	std::vector<ftxui::Element> elements;

	for (int verticalRow = 0; verticalRow <= TerminalHeight; verticalRow++)
	{
		bool foundLabel = false;

		for (const std::pair<int, double> volumeLabel : VolumeLabels)
		{
			if (volumeLabel.first == verticalRow)
			{
				elements.push_back(ftxui::text(reinterpret_cast<const char*>(u8"━┫")));
				foundLabel = true;
				break;
			}
		}

		if (!foundLabel)
		{
			elements.push_back(ftxui::text(reinterpret_cast<const char*>(u8" ┃")));
		}
	}

	return elements;
}

std::vector<ftxui::Element> SquareSeparator::BuildUpperBorder()
{
	std::vector<ftxui::Element> elements;

	for (size_t horizontalRow = 0; horizontalRow <= TerminalWidth; horizontalRow++)
	{

		if (horizontalRow == VolumeSymbolCount + BorderWidth - 1)
		{
			elements.push_back(ftxui::text(reinterpret_cast<const char*>(u8"┳")));
			continue;
		}
		if (horizontalRow == TerminalWidth - PriceSymbolCount - BorderWidth)
		{
			elements.push_back(ftxui::text(reinterpret_cast<const char*>(u8"┳")));
			continue;
		}

		elements.push_back(ftxui::text(reinterpret_cast<const char*>(u8"━")));
	}

	return elements;
}

std::vector<ftxui::Element> SquareSeparator::BuildLowerIntervalBorder()
{
	std::vector<ftxui::Element> elements;

	for (size_t horizontalRow = 0; horizontalRow <= TerminalWidth; horizontalRow++)
	{
		bool foundLabel = false;

		if (horizontalRow == VolumeSymbolCount + BorderWidth - 1)
		{
			elements.push_back(ftxui::text(reinterpret_cast<const char*>(u8"┻")));
			continue;
		}
		if (horizontalRow == TerminalWidth - PriceSymbolCount - BorderWidth)
		{
			elements.push_back(ftxui::text(reinterpret_cast<const char*>(u8"┻")));
			continue;
		}

		for (const std::pair<uint64_t, uint64_t> intervalLabel : IntervalLabels)
		{
			if (intervalLabel.first == horizontalRow)
			{
				elements.push_back(ftxui::text(reinterpret_cast<const char*>(u8"┳")));
				foundLabel = true;
				break;
			}
		}

		if (!foundLabel)
		{
			elements.push_back(ftxui::text(reinterpret_cast<const char*>(u8"━")));
		}
	}

	return elements;
}
