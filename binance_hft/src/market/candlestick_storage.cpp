#include "market/candlestick_storage.hpp"

CandlestickStorage& CandlestickStorage::Instance()
{
	static CandlestickStorage instance;
	return instance;
}

void CandlestickStorage::Upsert(Interval interval, const Candle& candle)
{
	std::scoped_lock<std::mutex> lock(Mutex);

	size_t IntervalIndex = static_cast<size_t>(interval);

	Storage[IntervalIndex][candle.CandlestickStartTime] = candle;

	if (Storage[IntervalIndex].size() > 750)
	{
		Storage[IntervalIndex].erase(Storage[IntervalIndex].begin());
	}
}

std::map<uint64_t, Candle> CandlestickStorage::GetCandles(Interval interval)
{
	std::scoped_lock<std::mutex> lock(Mutex);

	size_t IntervalIndex = static_cast<size_t>(interval);

	return Storage[IntervalIndex];
}

void CandlestickStorage::UpsertBatch(Interval interval, const std::vector<Candle>& candles)
{
	std::scoped_lock<std::mutex> lock(Mutex);

	size_t IntervalIndex = static_cast<size_t>(interval);

	for (const auto& candle : candles)
	{
		Storage[IntervalIndex][candle.CandlestickStartTime] = candle;

		if (Storage[IntervalIndex].size() > 750)
		{
			Storage[IntervalIndex].erase(Storage[IntervalIndex].begin());
		}
	}
}
