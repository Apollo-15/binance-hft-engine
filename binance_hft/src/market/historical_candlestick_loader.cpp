#include "market/historical_candlestick_loader.hpp"

#include "market/candlestick_storage.hpp"

#include <vector>
#include <future>

void LoadAllHistoricalCandlesticks(Binance::RestRequest& restRequest, const std::string& host)
{
	std::vector<std::future<void>> futures;
	futures.reserve(static_cast<size_t>(Interval::FinalBorder) * static_cast<size_t>(Symbol::FinalBorder));

	for (int j = 0; j < static_cast<int>(Symbol::FinalBorder); j++)
	{
		const Symbol currentSymbol = static_cast<Symbol>(j);

		for (int i = 0; i < static_cast<int>(Interval::FinalBorder); i++)
		{
			const Interval currentInterval = static_cast<Interval>(i);

			futures.push_back(std::async(std::launch::async, [&, currentInterval, currentSymbol]
				{
					std::vector<Candle> candles = restRequest.FetchHistoricalCandlesticks(currentInterval, currentSymbol, host);
					CandlestickStorage::Instance().UpsertBatch(currentInterval, currentSymbol, candles);

				})
			);
		}
	}

	for (auto& future : futures)
	{
		future.get();
	}
}
