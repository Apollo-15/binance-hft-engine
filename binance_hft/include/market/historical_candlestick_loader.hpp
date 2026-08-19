#pragma once

#include <string>

#include "rest/rest_requester.hpp"

void LoadAllHistoricalCandlesticks(Binance::RestRequest& restRequest, const std::string& host);
