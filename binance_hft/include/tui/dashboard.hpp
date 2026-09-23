#pragma once

#include "database/database.hpp"
#include "models/account_info.hpp"
#include "network/candlestick_websocket_client.hpp"
#include "network/websocket_client.hpp"
#include "market/symbol_utilities.hpp"
#include "network/internet_manager.hpp"
#include "network/latency_manager.hpp"
#include "portfolio/portfolio.hpp"
#include "trading/trade_buffer.hpp"

class DashBoard
{
private:
	Portfolio& PortfolioRef;
	DataBase& DataBaseRef;
	TradeBuffer& TradeBufferRef;
	AccountInfo& AccountInfoRef;
	std::shared_ptr<Binance::WebSocketClient>& ClientRef;
	InternetManager& InternetManagerRef;
	LatencyManager& LatencyManagerRef;
	std::array<std::shared_ptr<CandlestickWebSocketClient>, static_cast<size_t>(Symbol::FinalBorder)>& CandlestickClientRef;

public:

	explicit DashBoard(Portfolio& portfolioRef, DataBase& dataBaseRef, TradeBuffer& tradeBufferRef, AccountInfo& accountInfoRef,
		std::shared_ptr<Binance::WebSocketClient>& clientRef, InternetManager& internetManagerRef, LatencyManager& latencyManagerRef,
		std::array<std::shared_ptr<CandlestickWebSocketClient>, static_cast<size_t>(Symbol::FinalBorder)>&
		candlestickClientRef)
		: PortfolioRef(portfolioRef),
		  DataBaseRef(dataBaseRef),
		  TradeBufferRef(tradeBufferRef),
		  AccountInfoRef(accountInfoRef),
		  ClientRef(clientRef),
		  InternetManagerRef(internetManagerRef),
		  LatencyManagerRef(latencyManagerRef),
		  CandlestickClientRef(candlestickClientRef)
	{
	}

	void TuiStarter();
};
