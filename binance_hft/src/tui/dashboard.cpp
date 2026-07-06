#include "tui/dashboard.hpp"

#include "config/binance_config.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/component.hpp"  // for Radiobox, Renderer, Tab, Toggle, Vertical
#include "market/candlestick_storage.hpp"
#include "network/websocket_client.hpp"

#include <limits>
#include <algorithm>
#include <iostream>

void DashBoard::TuiStarter()
{
	auto screen = ftxui::ScreenInteractive::Fullscreen();

	const std::vector<std::string> tabNames
	{
		"Account",
		"Trades",
		"Candlesticks",
		"DataBase",
		"Logs",
		"Connection Status"
	};

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
			(void)localtime_s(&localTime, &time);

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

			double maxPrice = std::numeric_limits<double>::lowest();
			double minPrice = std::numeric_limits<double>::max();

			for (const auto& [timestamp, candle] 
				: CandlestickStorage::Instance().GetCandles(Interval::Min1))
			{
				if (candle.HighPrice > maxPrice)
				{
					maxPrice = candle.HighPrice;
				}

				if (candle.LowPrice < minPrice)
				{
					minPrice = candle.LowPrice;
				}	
			}

			int terminalHeight = screen.dimy() - 4;

			for (const auto& [timestamp, candle]
				: CandlestickStorage::Instance().GetCandles(Interval::Min1))
			{
				const double rowHigh = terminalHeight - ((candle.HighPrice - minPrice) / (maxPrice - minPrice) * terminalHeight);
				double rowOpen = terminalHeight - ((candle.OpenPrice - minPrice) / (maxPrice - minPrice) * terminalHeight);
				double rowClose = terminalHeight - ((candle.ClosePrice - minPrice) / (maxPrice - minPrice) * terminalHeight);
				const double rowLow = terminalHeight - ((candle.LowPrice - minPrice) / (maxPrice - minPrice) * terminalHeight);

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
					candleColumn.push_back(ftxui::text(row >= bodyTop && row <= bodyBottom ? 
						reinterpret_cast<const char*>(u8"▐█▌") : 
						reinterpret_cast<const char*>(u8" │")));
				}

				elements.push_back(ftxui::vbox(candleColumn) | 
					ftxui::color(!candle.IsCandlestickClosed ? 
					ftxui::Color::GrayDark :
					candle.ClosePrice > candle.OpenPrice ?
					ftxui::Color::Green : 
					ftxui::Color::Red));

			}

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
				const int labelRow = static_cast<int>(std::round(terminalHeight - (labelPrice -minPrice) / (maxPrice - minPrice) * terminalHeight));
				priceLabels.push_back({ labelRow, labelPrice });
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
						labelStream << std::fixed << std::setprecision(0) << priceLabel.second;
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
			auto candlestickChart = ftxui::hbox(elements);

			return ftxui::hbox({candlestickChart, ftxui::separator(), ftxui::vbox(priceLabelElements)});
		}
	);

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
				accountRenderer, tradeRenderer, candlestickRenderer , databaseRenderer, logsRenderer, connectionRenderer
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
