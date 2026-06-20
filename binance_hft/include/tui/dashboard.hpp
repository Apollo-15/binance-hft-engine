#pragma once
#include "database/database.hpp"
#include "models/account_info.hpp"
#include "portfolio/portfolio.hpp"
#include "trading/trade_buffer.hpp"

class DashBoard
{
private:
	Portfolio& PortfolioRef;
	DataBase& DataBaseRef;
	TradeBuffer& TradeBufferRef;
	AccountInfo& AccountInfoRef;

public:
	explicit DashBoard(Portfolio& portfolioRef, DataBase& dataBaseRef, TradeBuffer& tradeBufferRef, AccountInfo& accountInfoRef)
		: PortfolioRef(portfolioRef),
		  DataBaseRef(dataBaseRef),
		  TradeBufferRef(tradeBufferRef),
		  AccountInfoRef(accountInfoRef)
	{
	}

	void TuiStarter();
};
