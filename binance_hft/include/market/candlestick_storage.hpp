#pragma once

#include <map>
#include <mutex>
#include <array>
#include <vector>

#include "models/candle.hpp"
#include "candlestick_utilities.hpp"
#include "symbol_utilities.hpp"


class CandlestickStorage
{
private:
	mutable std::mutex Mutex;
	std::array<std::array<std::map<uint64_t, Candle>, 
		static_cast<size_t>(Interval::FinalBorder)>, 
		static_cast<size_t>(Symbol::FinalBorder)> Storage;

	CandlestickStorage() = default;

public:
	~CandlestickStorage() = default;

	static CandlestickStorage& Instance();

	CandlestickStorage(const CandlestickStorage& other) = delete;
	CandlestickStorage(CandlestickStorage&& other) noexcept = delete;
	CandlestickStorage& operator=(const CandlestickStorage& other) = delete;
	CandlestickStorage& operator=(CandlestickStorage&& other) noexcept = delete;

	void Upsert(Interval interval, Symbol symbol, const Candle& candle);
	void UpsertBatch(Interval interval, Symbol symbol, const std::vector<Candle>& candles);
	std::map<uint64_t, Candle> GetCandles(Interval interval, Symbol symbol);
};
