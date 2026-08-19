#pragma once

#include "database/database.hpp"
#include "models/account_info.hpp"
#include "network/candlestick_websocket_client.hpp"
#include "market/symbol_utilities.hpp"
#include "portfolio/portfolio.hpp"
#include "trading/trade_buffer.hpp"

class DashBoard
{
private:
	Portfolio& PortfolioRef;
	DataBase& DataBaseRef;
	TradeBuffer& TradeBufferRef;
	AccountInfo& AccountInfoRef;
	std::array<std::shared_ptr<CandlestickWebSocketClient>, static_cast<size_t>(Symbol::FinalBorder)>& CandlestickClientRef;

public:
	explicit DashBoard(Portfolio& portfolioRef, DataBase& dataBaseRef, TradeBuffer& tradeBufferRef, AccountInfo& accountInfoRef,
		std::array<std::shared_ptr<CandlestickWebSocketClient>, static_cast<size_t>(Symbol::FinalBorder)>& candlestickClientRef)
		: PortfolioRef(portfolioRef),
		  DataBaseRef(dataBaseRef),
		  TradeBufferRef(tradeBufferRef),
		  AccountInfoRef(accountInfoRef),
		  CandlestickClientRef(candlestickClientRef)
	{
	}

	void TuiStarter();
};
