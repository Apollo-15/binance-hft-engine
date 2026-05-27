#pragma once

#include <mutex>
#include <sqlite3.h>

#include "models/trade_batch.hpp"
#include "models/trade_event.hpp"

class DataBase
{
private:
	sqlite3* Db = nullptr;
	mutable std::mutex Mutex;
	
public:
	[[nodiscard]]
	bool IsConnectionOpened() const;
	bool OpenConnection(const std::string&  filename);
	void CreateTradesTable();
	void CreateBatchesTable();
	void CreatePortfolioTable();
	void SaveTrade(const TradeEvent& trade);
	void SaveBatch(const TradeBatch& batch);
	void UpdatePortfolio();
};