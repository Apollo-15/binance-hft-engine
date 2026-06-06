#include <database/database.hpp>
#include <sqlite3.h>

#include "portfolio/portfolio.hpp"

bool DataBase::IsConnectionOpened() const
{
	std::scoped_lock<std::mutex> lock(Mutex);

	return Db != nullptr;
}

bool DataBase::OpenConnection(const std::string& filename)
{
	std::scoped_lock<std::mutex> lock(Mutex);

	auto isOk = sqlite3_open_v2(
		filename.c_str(), 
		&Db, 
		SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 
		nullptr
	);

	return isOk == SQLITE_OK;
}

void DataBase::CreatePortfolioTable()
{
	std::scoped_lock<std::mutex> lock(Mutex);

	const char* sql = "CREATE TABLE IF NOT EXISTS portfolio "
				   "(symbol TEXT UNIQUE, balance REAL, total_position REAL)";

	sqlite3_exec(
		Db,
		sql,
		nullptr,
		nullptr,
		nullptr
	);
}

void DataBase::CreateBatchesTable()
{
	std::scoped_lock<std::mutex> lock(Mutex);

	const char* sql = "CREATE TABLE IF NOT EXISTS batches "
				   "(id INTEGER PRIMARY KEY, batch_id INTEGER UNIQUE, is_sold INTEGER, pnl REAL)";

	sqlite3_exec(
		Db,
		sql,
		nullptr,
		nullptr,
		nullptr
	);
}

void DataBase::CreateTradesTable()
{
	std::scoped_lock<std::mutex> lock(Mutex);

	const char* sql = "CREATE TABLE IF NOT EXISTS trades "
				   "(id INTEGER PRIMARY KEY, trade_id INTEGER, symbol TEXT, price REAL, quantity REAL, trade_time INTEGER)";

	sqlite3_exec(
		Db,
		sql,
		nullptr,
		nullptr,
		nullptr
	);
}

void DataBase::SaveTrade(const TradeEvent& trade)
{
	std::scoped_lock<std::mutex> lock(Mutex);

	const char* sql = "INSERT INTO trades "
		"(trade_id, symbol, price, quantity, trade_time) VALUES (?, ?, ?, ?, ?)";

	sqlite3_stmt* stmt = nullptr;

	sqlite3_prepare_v2(
		Db,
		sql,
		-1,
		&stmt,
		nullptr
	);

	sqlite3_bind_int64(stmt, 1, trade.TradeId);
	sqlite3_bind_text(stmt, 2, trade.Symbol.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_double(stmt, 3, trade.Price);
	sqlite3_bind_double(stmt, 4, trade.Quantity);
	sqlite3_bind_int64(stmt, 5, trade.TradeTime);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);
}

void DataBase::SaveBatch(const TradeBatch& batch)
{
	std::scoped_lock<std::mutex> lock(Mutex);

	const char* sql = "INSERT OR REPLACE INTO batches "
		"(batch_id, is_sold, pnl) VALUES (?, ?, ?)";

	sqlite3_stmt* stmt = nullptr;

	sqlite3_prepare_v2(
		Db,
		sql,
		-1,
		&stmt,
		nullptr
	);

	sqlite3_bind_int64(stmt, 1, batch.BatchId);
	sqlite3_bind_int64(stmt, 2, batch.IsSold ? 1 : 0);
	sqlite3_bind_double(stmt, 3, batch.PnL);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);
}

void DataBase::UpdatePortfolio()
{
	std::scoped_lock<std::mutex> lock(Mutex);

	const char* sql = "INSERT OR REPLACE INTO portfolio "
		"(symbol, balance, total_position) VALUES (?, ?, ?)";

	sqlite3_stmt* stmt = nullptr;

	sqlite3_prepare_v2(
		Db,
		sql,
		-1,
		&stmt,
		nullptr
	);

	sqlite3_bind_text(stmt, 1, "BTCUSDT", -1, SQLITE_STATIC);
	sqlite3_bind_double(stmt, 2, Portfolio::Instance(50000.00).GetBalance());
	sqlite3_bind_double(stmt, 3, Portfolio::Instance(50000.00).GetPosition());
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);
}

std::vector<TradeEvent> DataBase::ReadTrades()
{
	std::scoped_lock<std::mutex> lock(Mutex);
	std::vector<TradeEvent> event;

	const char* sql = "SELECT * FROM trades";

	sqlite3_stmt* stmt = nullptr;

	sqlite3_prepare_v2(
		Db,
		sql,
		-1,
		&stmt,
		nullptr
	);

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		TradeEvent trade;
		const auto id = sqlite3_column_int64(stmt, 1);
		const auto symbol = sqlite3_column_text(stmt, 2);
		const auto price = sqlite3_column_double(stmt, 3);
		const auto quantity = sqlite3_column_double(stmt, 4);
		const auto tradeTime = sqlite3_column_int64(stmt, 5);

		trade.TradeId = id;
		trade.Symbol = reinterpret_cast<const char*>(symbol);
		trade.Price = price;
		trade.Quantity = quantity;
		trade.TradeTime = tradeTime;

		event.push_back(trade);
	}
	sqlite3_finalize(stmt);

	return event;
}
