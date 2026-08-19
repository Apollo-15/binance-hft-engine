#include "tui/dashboard.hpp"

#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/component.hpp"  // for Checkbox,Radiobox, Renderer, Tab, Toggle, Vertical
#include "market/symbol_utilities.hpp"
#include "market/candlestick_storage.hpp"
#include "network/websocket_client.hpp"
#include "tui/decorators/square_separator.hpp"

#include <limits>
#include <algorithm>

#include "tui/widgets/wheel_picker.hpp"

void DashBoard::TuiStarter()
{
	auto screen = ftxui::ScreenInteractive::Fullscreen();

	int intervalSelected = 0;
	int symbolSelected = 0;

	std::vector<std::string> intervalEntries;
	std::vector<std::string> symbolEntries;

	constexpr auto intervalFinalBorder = static_cast<int>(Interval::FinalBorder);
	constexpr auto symbolFinalBorder = static_cast<int>(Symbol::FinalBorder);
	
	intervalEntries.reserve(intervalFinalBorder);
	symbolEntries.reserve(symbolFinalBorder);

	for (int i = 0; i < intervalFinalBorder; i++)
	{
		intervalEntries.push_back(static_cast<std::string>(IntervalToDisplayString(static_cast<Interval>(i))));
	}

	for (int i = 0; i < symbolFinalBorder; i++)
	{
		symbolEntries.push_back(static_cast<std::string>(SymbolToDisplayString(static_cast<Symbol>(i))));
	}

	const std::vector<std::string> tabNames
	{
		"Account",
		"Trades",
		"Candlesticks",
		"DataBase",
		"Logs",
		"Connection Status"
	};

	ftxui::Component intervalWheelPicker = MakeWheelPicker(&intervalEntries, &intervalSelected);
	ftxui::Component symbolWheelPicker = MakeWheelPicker(&symbolEntries, &symbolSelected);

	ftxui::Component buttonHandler = ftxui::Renderer(ftxui::Container::Horizontal({ intervalWheelPicker, symbolWheelPicker }), [&]
		{
			return ftxui::hbox({
				intervalWheelPicker->Render(),
				ftxui::filler(),
				symbolWheelPicker->Render()
				});
		});

	int tabSelected = 0;
	auto tabToggle = ftxui::Toggle(&tabNames, &tabSelected);

	auto accountRenderer = ftxui::Renderer([&] 
		{
		std::vector<ftxui::Element> elements;
		elements.push_back(ftxui::text(std::to_string(AccountInfoRef.AccountId)));

		for (auto position : AccountInfoRef.Positions)
		{
			elements.push_back(ftxui::text(position.Symbol));
			elements.push_back(ftxui::text(std::to_string(position.Price)));
			elements.push_back(ftxui::text(std::to_string(position.Quantity)));
			elements.push_back(ftxui::text(std::to_string(position.TotalValue)));
			elements.push_back(ftxui::text(std::to_string(position.TotalQuantity)));
		}
		return ftxui::vbox(elements);
		}
	);

	auto tradeRenderer = ftxui::Renderer([&] 
		{
		std::vector<ftxui::Element> elements;

		for (TradeEvent event : TradeBufferRef.GetTradeDeque())
		{
			std::vector<ftxui::Element> tradeRow;

			std::time_t time = event.TradeTime / 1000;
			std::tm localTime;
			static_cast<void>(localtime_s(&localTime, &time));

			std::ostringstream timeStream;
			std::ostringstream priceStream;
			std::ostringstream quantityStream;

			timeStream << std::put_time(&localTime, "%H:%M:%S");
			priceStream << std::fixed << std::setprecision(8) << event.Price;
			quantityStream << std::fixed << std::setprecision(8) << event.Quantity;

			tradeRow.push_back(ftxui::text("ID: " + std::to_string(event.TradeId)));
			tradeRow.push_back(ftxui::text(" | Symbol: " + event.Symbol));
			tradeRow.push_back(ftxui::text(" | Price: " + priceStream.str()));
			tradeRow.push_back(ftxui::text(" | Quantity: " + quantityStream.str()));
			tradeRow.push_back(ftxui::text(" | Time: " + timeStream.str()));

			auto tradeElements = ftxui::hbox(tradeRow);
			elements.push_back(tradeElements);
		}

		return ftxui::vbox(elements);
		}
	);

	auto candlestickRenderer = ftxui::Renderer([&]
		{	
			std::vector<ftxui::Element> elements;
			std::vector<std::pair<uint64_t, uint64_t>> intervalLabels;

			double maxPrice = std::numeric_limits<double>::lowest();
			double minPrice = std::numeric_limits<double>::max();

			double maxVolume = std::numeric_limits<double>::lowest();
			double minVolume = std::numeric_limits<double>::max();

			for (const auto& [timestamp, candle] 
				: CandlestickStorage::Instance().GetCandles(static_cast<Interval>(intervalSelected), static_cast<Symbol>(symbolSelected)))
			{
				maxPrice = std::max(maxPrice, candle.HighPrice);
				minPrice = std::min(minPrice, candle.LowPrice);

				maxVolume = std::max(maxVolume, candle.BaseAssetVolume);
				minVolume = std::min(minVolume, candle.BaseAssetVolume);
			}

			// Candlestick Renderer;
				
			std::ostringstream priceLength;
			std::ostringstream volumeLength;

			priceLength << std::fixed << std::setprecision(0) << maxPrice;
			volumeLength << std::fixed << std::setprecision(0) << maxVolume;

			// Candlestick Renderer;

			const uint64_t totalCandlesSize = CandlestickStorage::Instance().GetCandles(static_cast<Interval>(intervalSelected), static_cast<Symbol>(symbolSelected)).size();
			const uint64_t availableWidthForCandles = screen.dimx() - 2 - 2 - 1 - priceLength.str().size() - volumeLength.str().size();
			const uint64_t maxVisibleCandles = availableWidthForCandles / 3;
			const uint64_t unseenCandlesCount = std::max<uint64_t>(0, totalCandlesSize - maxVisibleCandles);

			const int terminalWidth = screen.dimx() - 3;
			const int terminalHeight = screen.dimy() - 11;

			uint64_t index = 0;

			for (const auto& [timestamp, candle]
				: CandlestickStorage::Instance().GetCandles(static_cast<Interval>(intervalSelected), static_cast<Symbol>(symbolSelected)))
			{
				if (index < unseenCandlesCount)
				{
					index++;
					continue;
				}

				const double rowHigh = terminalHeight - ((candle.HighPrice - minPrice) / (maxPrice - minPrice) * terminalHeight);
				const double rowOpen = terminalHeight - ((candle.OpenPrice - minPrice) / (maxPrice - minPrice) * terminalHeight);
				const double rowClose = terminalHeight - ((candle.ClosePrice - minPrice) / (maxPrice - minPrice) * terminalHeight);
				const double rowLow = terminalHeight - ((candle.LowPrice - minPrice) / (maxPrice - minPrice) * terminalHeight);

				const double volumeBarRow = terminalHeight - ((candle.BaseAssetVolume - minVolume) / (maxVolume - minVolume) * terminalHeight);

				const double bodyTop = std::floor(std::min(rowOpen, rowClose));
				const double bodyBottom = std::ceil(std::max(rowOpen, rowClose));

				std::vector<ftxui::Element> candleColumn;

				for (int emptyRow = 0; emptyRow < static_cast<int>(std::floor(rowHigh)); emptyRow++)
				{
					candleColumn.push_back(ftxui::text(" "));
				}

				for (int row = static_cast<int>(std::floor(rowHigh));
					row <= static_cast<int>(std::ceil(rowLow)); row++)
				{
					candleColumn.push_back(ftxui::text(
						row >= bodyTop && row <= bodyBottom ?
							(row >= volumeBarRow ?
								reinterpret_cast<const char*>(u8"▐░▌") :
								reinterpret_cast<const char*>(u8"▐█▌")) 
						:
							(row >= volumeBarRow ?
								reinterpret_cast<const char*>(u8" ┆") :
								reinterpret_cast<const char*>(u8" │"))
					));
				}

				for (int row = static_cast<int>(std::ceil(rowLow)) + 1;
					 row <= terminalHeight; row++)
				{
					if (row >= volumeBarRow)
					{
						candleColumn.push_back(ftxui::text(reinterpret_cast<const char*>(u8"▐█▌")));
					}
					else
					{
						candleColumn.push_back(ftxui::text(" "));
					}
				}

				elements.push_back(ftxui::vbox(candleColumn) |
					ftxui::color(!candle.IsCandlestickClosed ?
						ftxui::Color::GrayDark :
						candle.ClosePrice > candle.OpenPrice ?
						ftxui::Color::Green :
						ftxui::Color::Red));

				if (index % 7 == 0)
				{
					intervalLabels.push_back({ (index - unseenCandlesCount) * 3 + 1 + volumeLength.str().size() + 2, timestamp });
				}

				index++;
			}

			// Price labels

			double niceRoundedFraction;

			const int desiredLabelCount = terminalHeight / 3;
			const double rawStep = (maxPrice - minPrice) / desiredLabelCount;
			const double exponentValue = std::floor(std::log10(rawStep));
			const double fractionalPart = rawStep / std::pow(10.0, exponentValue);

			if (fractionalPart <= 1)
			{
				niceRoundedFraction = 1;
			}
			else if (fractionalPart <= 2)
			{
				niceRoundedFraction = 2;
			}
			else if (fractionalPart <= 5)
			{
				niceRoundedFraction = 5;
			}
			else
			{
				niceRoundedFraction = 10;
			}

			const double step = niceRoundedFraction * std::pow(10.0, exponentValue);
			double labelPrice = std::ceil(minPrice / step) * step;
			
			std::vector<std::pair<int, double>> priceLabels;

			while (labelPrice <= maxPrice)
			{
				const int labelRow = static_cast<int>(std::round(terminalHeight - (labelPrice - minPrice) / (maxPrice - minPrice) * terminalHeight));

				if (labelRow != 0 && labelRow != terminalHeight - 1)
				{
					priceLabels.push_back({ labelRow, labelPrice });
				}
				labelPrice += step;
			}

			std::vector<ftxui::Element> priceLabelElements;

			for (int row = 0; row <= terminalHeight; row++)
			{
				bool foundLabel = false;
				std::ostringstream labelStream;

				for (const auto & priceLabel : priceLabels)
				{
					if (priceLabel.first == row)
					{
						labelStream << std::fixed << "$" << std::setprecision(0) << priceLabel.second;
						priceLabelElements.push_back(ftxui::text(labelStream.str()));
						foundLabel = true;
						break;
					}
				}
				if (!foundLabel)
				{
					priceLabelElements.push_back(ftxui::text(" "));
				}
			}
			const ftxui::Element candlestickChart = ftxui::hbox(elements);

			// Volume labels

			const int volumeZoneRow = terminalHeight - (terminalHeight / 4);

			double niceRoundedVolumeFraction;

			const int desiredVolumeLabelCount = terminalHeight / 3;
			const double rawVolumeStep = (maxVolume - minVolume) / desiredVolumeLabelCount;
			const double exponentVolumeValue = std::floor(std::log10(rawVolumeStep));
			const double fractionalVolumePart = rawVolumeStep / std::pow(10.0, exponentVolumeValue);

			if (fractionalVolumePart <= 1)
			{
				niceRoundedVolumeFraction = 1;
			}
			else if (fractionalVolumePart <= 2)
			{
				niceRoundedVolumeFraction = 2;
			}
			else if (fractionalVolumePart <= 5)
			{
				niceRoundedVolumeFraction = 5;
			}
			else
			{
				niceRoundedVolumeFraction = 10;
			}

			const double volumeStep = niceRoundedVolumeFraction * std::pow(10.0, exponentVolumeValue);
			double labelVolume = std::ceil(minVolume / volumeStep) * volumeStep;

			std::vector<std::pair<int, double>> volumeLabels;

			while (labelVolume <= maxVolume)
			{
				const int labelVolumeRow = static_cast<int>(std::round(terminalHeight - (labelVolume - minVolume) / (maxVolume - minVolume) * terminalHeight));
				if (labelVolumeRow != 0 && labelVolumeRow != terminalHeight - 1)
				{
					volumeLabels.emplace_back(labelVolumeRow, labelVolume);
				}
				labelVolume += volumeStep;
			}

			std::vector<ftxui::Element> volumeLabelElements;

			for (int row = 0; row <= terminalHeight; row++)
			{
				bool foundLabel = false;
				std::ostringstream labelStream;

				for (const auto& volumeLabel : volumeLabels)
				{
					if (volumeLabel.first == row)
					{
						labelStream << std::fixed << std::setprecision(0) << volumeLabel.second;
						volumeLabelElements.push_back(ftxui::text(labelStream.str()));
						foundLabel = true;
						break;
					}
				}
				if (!foundLabel)
				{
					volumeLabelElements.push_back(ftxui::text(" "));
				}
			}

			// Interval labels

			const char* dateFormat;

			if (static_cast<Interval>(intervalSelected) <= Interval::Min30)
			{
				dateFormat = "%b %d %I:%M:%S %p";
			}
			else if (static_cast<Interval>(intervalSelected) <= Interval::Hr12)
			{
				dateFormat = "%b %d %I:%M %p";
			}
			else
			{
				dateFormat = "%b %d %Y";
			}

			std::vector<ftxui::Element> intervalLabelElements;

			for (int64_t column = 0; column <= terminalWidth; column++)
			{
				bool foundLabel = false;

				for (const auto& intervalLabel : intervalLabels)
				{
					std::time_t time = static_cast<std::time_t>(intervalLabel.second) / 1000;
					std::tm localTime;
					static_cast<void>(localtime_s(&localTime, &time));
					std::ostringstream timeStream;
					timeStream << std::put_time(&localTime, dateFormat);
					const int64_t centralizedLabel = static_cast<int64_t>(intervalLabel.first - timeStream.str().size() / 2);

					if (column >= centralizedLabel && static_cast<size_t>(column) <= centralizedLabel + timeStream.str().size() - 1)
					{
						intervalLabelElements.push_back(ftxui::text(std::string(1, 
							timeStream.str().at(column - centralizedLabel))));

						foundLabel = true;
						break;
					}
				}
				if (!foundLabel)
				{
					intervalLabelElements.push_back(ftxui::text(" "));
				}
			}

			SquareSeparator squareSeparator(
				terminalHeight, terminalWidth,
				volumeZoneRow,
				priceLabels, volumeLabels, intervalLabels,
				volumeLength.str().size(), priceLength.str().size(),
				2
			);

			return ftxui::vbox({
				ftxui::hbox(squareSeparator.BuildUpperBorder()),
				ftxui::hbox({
					ftxui::vbox(volumeLabelElements),
					ftxui::vbox(squareSeparator.BuildLeftVolumeBorder()),
					candlestickChart | ftxui::flex ,
					ftxui::vbox(squareSeparator.BuildRightPriceBorder()),
					ftxui::vbox(priceLabelElements)
				}),
				ftxui::hbox(squareSeparator.BuildLowerIntervalBorder()),
				ftxui::hbox(intervalLabelElements)
			});
		}
	);

	auto candlestickTab = ftxui::Container::Vertical({ buttonHandler, candlestickRenderer });

	auto databaseRenderer = ftxui::Renderer([&]
		{
		std::vector<ftxui::Element> elements;

		for (auto event : DataBaseRef.ReadTrades())
		{
			std::vector<ftxui::Element> dbRow;

			std::time_t time = event.TradeTime / 1000;
			std::tm localTime;
			(void)localtime_s(&localTime, &time);

			std::ostringstream timeStream;
			std::ostringstream priceStream;
			std::ostringstream quantityStream;

			timeStream << std::put_time(&localTime, "%H:%M:%S");
			priceStream << std::fixed << std::setprecision(8) << event.Price;
			quantityStream << std::fixed << std::setprecision(8) << event.Quantity;

			dbRow.push_back(ftxui::text("ID: " + std::to_string(event.TradeId)));
			dbRow.push_back(ftxui::text(" | Symbol: " + event.Symbol));
			dbRow.push_back(ftxui::text(" | Price: " + priceStream.str()));
			dbRow.push_back(ftxui::text(" | Quantity: " + quantityStream.str()));
			dbRow.push_back(ftxui::text(" | Time: " + timeStream.str()));

			auto dbElements = ftxui::hbox(dbRow);
			elements.push_back(dbElements);
		}
		return ftxui::vbox(elements);
		}
	);

	auto logsRenderer = ftxui::Renderer([&] 
		{
			return ftxui::vbox(ftxui::text("Logs coming soon!"));
		}
	);

	auto connectionRenderer = ftxui::Renderer([&]
		{
			return ftxui::vbox(ftxui::text("Connection Status coming soon!"));
		}
	);

	const ftxui::Component tabContainer = ftxui::Container::Tab(
		{
				accountRenderer, tradeRenderer, candlestickTab , databaseRenderer, logsRenderer, connectionRenderer
		},
	&tabSelected);

	const auto container = ftxui::Container::Vertical(
		{
			tabToggle,
			tabContainer
		}
	);

	const auto renderer = ftxui::Renderer(container, [&]
		{
			return ftxui::vbox({
				tabToggle->Render(),
				ftxui::separator(),
				tabContainer->Render()
				}) | ftxui::border;
		}
	);

	screen.Loop(renderer);
}
