#include "market/historical_candlestick_loader.hpp"

#include "market/candlestick_storage.hpp"
#include <vector>
#include <future>
#include <functional>
#include <chrono>
#include <thread>

void HistoricalCandlestickLoader::Start()
{
    StopRequest = false;
    BIsRunning = true;
    HistoricalCandlestickThread = std::thread(&HistoricalCandlestickLoader::LoadAllHistoricalCandlesticks, this);
}

void HistoricalCandlestickLoader::Stop()
{
    BIsRunning = false;
    if (HistoricalCandlestickThread.joinable())
    {
        StopRequest = true;
        HistoricalCandlestickThread.join();
    }
}

void HistoricalCandlestickLoader::LoadAllHistoricalCandlesticks()
{
	std::vector<std::future<void>> futures;
	futures.reserve(static_cast<size_t>(Interval::FinalBorder) * static_cast<size_t>(Symbol::FinalBorder));

	for (int j = 0; j < static_cast<int>(Symbol::FinalBorder); j++)
	{
		const Symbol currentSymbol = static_cast<Symbol>(j);
	    if (StopRequest == true)
	    {
	        break;
	    }

		for (int i = 0; i < static_cast<int>(Interval::FinalBorder); i++)
		{
		    if (StopRequest == true)
		    {
		        break;
		    }

			const Interval currentInterval = static_cast<Interval>(i);

			futures.push_back(std::async(std::launch::async, [&, currentInterval, currentSymbol]
				{
			        while (StopRequest == false)
			        {
			            std::vector<Candle> candles = RestRequest.FetchHistoricalCandlesticks(currentInterval, currentSymbol, Host);

			            if (candles.size() == 0)
			            {
                            std::this_thread::sleep_for(std::chrono::milliseconds(500));
			            }
			            else
			            {
			                CandlestickStorage::Instance().UpsertBatch(currentInterval, currentSymbol, candles);
			                break;
			            }
			        }
				})
			);
		}
	}

	for (auto& future : futures)
	{
		future.get();
	}
}
