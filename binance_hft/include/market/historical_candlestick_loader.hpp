#pragma once

#include <string>
#include <thread>
#include <atomic>

#include "rest/rest_requester.hpp"

class HistoricalCandlestickLoader
{
private:
    Binance::RestRequest& RestRequest;
    const std::string Host;
    std::thread HistoricalCandlestickThread;
    std::atomic<bool> BIsRunning { true };
    std::atomic<bool> StopRequest { true };

public:
    HistoricalCandlestickLoader(Binance::RestRequest& restRequest, const std::string& host)
        : RestRequest(restRequest),
          Host(host)
    {
    }

    void Start();
    void Stop();
    void LoadAllHistoricalCandlesticks();
};