#include "tui/dashboard.hpp"

#include "config/binance_config.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/component.hpp"  // for Radiobox, Renderer, Tab, Toggle, Vertical
#include "network/websocket_client.hpp"

void DashBoard::TuiStarter()
{
	std::vector<std::string> tabNames
	{
		"Account",
		"Trades",
		"DataBase",
		"Logs",
		"Connection Status"
	};

	int tabSelected = 0;
	auto tabToggle = ftxui::Toggle(&tabNames, &tabSelected);

	auto accountRenderer = ftxui::Renderer([&] 
		{
		std::vector<ftxui::Element> elements;
		elements.push_back(ftxui::text(AccountInfoRef.AccountName));
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
				accountRenderer, tradeRenderer, databaseRenderer, logsRenderer, connectionRenderer
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

	auto screen = ftxui::ScreenInteractive::Fullscreen();

	screen.Loop(renderer);
}
