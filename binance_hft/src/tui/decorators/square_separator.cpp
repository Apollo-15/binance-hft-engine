#include "tui/decorators/square_separator.hpp"

std::vector<ftxui::Element> SquareSeparator::BuildRightBorder()
{
	std::vector<ftxui::Element> elements;

	for (int row = 0; row <= TerminalHeight; row++)
	{
		bool foundLabel = false;

		for (const std::pair<int, double> priceLabel : PriceLabels)
		{
			if (priceLabel.first == row)
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
