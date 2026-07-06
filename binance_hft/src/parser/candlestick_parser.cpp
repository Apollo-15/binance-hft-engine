#include <../include/simdjson.h>
#include <iostream>

#include "parser/candlestick_parser.hpp"

namespace OnDemand = simdjson::ondemand;

std::optional<Candle> JsonParser::ParseCandlestick(const std::string& json)
{
	try
	{
		std::string_view view;
		std::string jsonCopy = json;
		OnDemand::parser parser;
		OnDemand::document doc = parser.iterate(simdjson::pad(jsonCopy));
		
		auto candlestick = doc["data"]["k"].get_object();

		const uint64_t candlestickStartTime = candlestick["t"].get_uint64();
		const double openPrice = candlestick["o"].get_double_in_string();
		const double closePrice = candlestick["c"].get_double_in_string();
		const double highPrice = candlestick["h"].get_double_in_string();
		const double lowPrice = candlestick["l"].get_double_in_string();
		const double baseAssetVolume = candlestick["v"].get_double_in_string();
		const uint64_t numberOfTrades = candlestick["n"].get_uint64();
		const bool isCandlestickClosed = candlestick["x"].get_bool();
		const uint64_t candlestickCloseTime = candlestick["T"].get_uint64();
		candlestick["i"].get(view);

		return Candle
		{
			.CandlestickStartTime = candlestickStartTime,
			.OpenPrice = openPrice,
			.ClosePrice = closePrice,
			.HighPrice = highPrice,
			.LowPrice = lowPrice,
			.BaseAssetVolume = baseAssetVolume,
			.NumberOfTrades = numberOfTrades,
			.IsCandlestickClosed = isCandlestickClosed,
			.CandlestickCloseTime = candlestickCloseTime,
			.Interval = IntervalFromString(view)
		};
	}
	catch (const std::exception& ex)
	{
		std::cerr << "[Candlestick Parser] Error happened: " << ex.what() << '\n';
		return std::nullopt;
	}
}

std::optional<std::vector<Candle>> JsonParser::ParseHistoricalCandlestick(const std::string& json, Interval interval)
{
	try
	{
		std::string jsonCopy = json;
		OnDemand::parser parser;
		OnDemand::document doc = parser.iterate(simdjson::pad(jsonCopy));

		auto candleStickArray = doc.get_array();

		std::vector<Candle> result;

		for (auto candleStick : candleStickArray)
		{
			auto singleCandlestick = candleStick.get_array();

			uint64_t candleStickStartTime = 0;
			double openPrice = 0;
			double highPrice = 0;
			double lowPrice = 0;
			double closePrice = 0;
			double baseAssetVolume = 0;
			uint64_t candleStickCloseTime = 0;
			uint64_t numberOfTrades = 0;

			auto index = 0;
			for (auto field : singleCandlestick)
			{
				switch (index)
				{
					case 0:
						candleStickStartTime = field.get_uint64();
						break;
					case 1:
						openPrice = field.get_double_in_string();
						break;
					case 2:
						highPrice = field.get_double_in_string();
						break;
					case 3:
						lowPrice = field.get_double_in_string();
						break;
					case 4:
						closePrice = field.get_double_in_string();
						break;
					case 5:
						baseAssetVolume = field.get_double_in_string();
						break;
					case 6:
						candleStickCloseTime = field.get_uint64();
						break;
					case 8:
						numberOfTrades = field.get_uint64();
						break;
					default:
						break;
				}			

				index++;
			}

			result.push_back(
				Candle
				{
					.CandlestickStartTime = candleStickStartTime,
					.OpenPrice = openPrice,
					.ClosePrice = closePrice,
					.HighPrice = highPrice,
					.LowPrice = lowPrice,
					.BaseAssetVolume = baseAssetVolume,
					.NumberOfTrades = numberOfTrades,
					.IsCandlestickClosed = true,
					.CandlestickCloseTime = candleStickCloseTime,
					.Interval = interval
				}
			);
		}

		return result;
	}
	catch (const std::exception& ex)
	{
		std::cerr << "[Historical Candlestick Parser] Error happened: " << ex.what() << '\n';
		return std::nullopt;
	}
}
