#pragma once

#include <vector>
#include <ftxui/dom/elements.hpp>

class SquareSeparator
{
private:
	const int TerminalHeight;
	const size_t TerminalWidth;
	const int VolumeZoneRow;
	const std::vector<std::pair<int, double>> PriceLabels;
	const std::vector<std::pair<int, double>> VolumeLabels;
	const std::vector<std::pair<uint64_t, uint64_t>> IntervalLabels;

	const size_t VolumeSymbolCount;
	const size_t PriceSymbolCount;

	const size_t BorderWidth;

	explicit SquareSeparator(const int terminalHeight, const size_t terminalWidth, const int volumeZoneRow,
		std::vector<std::pair<int, double>> priceLabels, std::vector<std::pair<int, double>> volumeLabels,
		std::vector<std::pair<uint64_t, uint64_t>> intervalLabels, const size_t volumeSymbolCount,
		const size_t priceSymbolCount, const int borderWidth)
		: TerminalHeight(terminalHeight),
		  TerminalWidth(terminalWidth),
		  VolumeZoneRow(volumeZoneRow),
		  PriceLabels(std::move(priceLabels)),
		  VolumeLabels(std::move(volumeLabels)),
		  IntervalLabels(std::move(intervalLabels)),
		  VolumeSymbolCount(volumeSymbolCount),
		  PriceSymbolCount(priceSymbolCount),
		  BorderWidth(borderWidth)
	{
	}

	friend class DashBoard;

public:
	std::vector<ftxui::Element> BuildRightPriceBorder();
	std::vector<ftxui::Element> BuildLeftVolumeBorder();
	std::vector<ftxui::Element> BuildUpperBorder();
	std::vector<ftxui::Element> BuildLowerIntervalBorder();

};
