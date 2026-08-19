#include "market/candlestick_storage.hpp"

CandlestickStorage& CandlestickStorage::Instance()
{
	static CandlestickStorage instance;
	return instance;
}

void CandlestickStorage::Upsert(Interval interval, Symbol symbol, const Candle& candle)
{
	std::scoped_lock<std::mutex> lock(Mutex);

	const size_t intervalIndex = static_cast<size_t>(interval);
	const size_t symbolIndex = static_cast<size_t>(symbol);

	Storage[symbolIndex][intervalIndex][candle.CandlestickStartTime] = candle;

	if (Storage[symbolIndex][intervalIndex].size() > 750)
	{
		Storage[symbolIndex][intervalIndex].erase(Storage[symbolIndex][intervalIndex].begin());
	}
}

std::map<uint64_t, Candle> CandlestickStorage::GetCandles(Interval interval, Symbol symbol)
{
	std::scoped_lock<std::mutex> lock(Mutex);

	const size_t intervalIndex = static_cast<size_t>(interval);
	const size_t symbolIndex = static_cast<size_t>(symbol);

	return Storage[symbolIndex][intervalIndex];
}

void CandlestickStorage::UpsertBatch(Interval interval, Symbol symbol, const std::vector<Candle>& candles)
{
	std::scoped_lock<std::mutex> lock(Mutex);

	const size_t intervalIndex = static_cast<size_t>(interval);
	const size_t symbolIndex = static_cast<size_t>(symbol);

	for (const auto& candle : candles)
	{
		Storage[symbolIndex][intervalIndex][candle.CandlestickStartTime] = candle;

		if (Storage[symbolIndex][intervalIndex].size() > 750)
		{
			Storage[symbolIndex][intervalIndex].erase(Storage[symbolIndex][intervalIndex].begin());
		}
	}
}
