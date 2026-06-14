#include "config/config_reader.hpp"
#include <../include/simdjson.h>

#include <fstream>
#include <iostream>

namespace OnDemand = simdjson::ondemand;

Binance::BinanceConfig Binance::ReadConfig(const std::string& path)
{
	std::ifstream inputFileStream(path, std::ios::binary);

	if (!inputFileStream.is_open())
	{
		std::cout << "Failed to open " << path << '\n';
		return BinanceConfig {};
	}
	else
	{
		const std::istreambuf_iterator<char> it{ inputFileStream }, end;
		const std::string ss{ it, end };

		try
		{
			std::string_view restHostView;
			std::string_view restPortView;
			std::string_view wsHostView;
			std::string_view wsPortView;
			std::string_view apiView;
			std::string_view secretKeyView;
			std::string jsonCopy = ss;
			OnDemand::parser parser;
			OnDemand::document doc = parser.iterate(simdjson::pad(jsonCopy));

			doc["RestHost"].get(restHostView);
			doc["RestPort"].get(restPortView);
			doc["WsHost"].get(wsHostView);
			doc["WsPort"].get(wsPortView);
			doc["APIKey"].get(apiView);
			doc["SecretKey"].get(secretKeyView);

			int64_t capValue = doc["Cap"].get_int64();
			int64_t baseDelayValue = doc["BaseDelay"].get_int64();
			int64_t jitterMaxValue = doc["JitterMax"].get_int64();
			auto baseDelay = std::chrono::seconds(baseDelayValue);
			auto jitterMax = std::chrono::seconds(jitterMaxValue);
			auto cap = std::chrono::seconds(capValue);

			return BinanceConfig
			{
				.RestHost = std::string(restHostView),
				.RestPort = std::string(restPortView),
				.WsHost = std::string(wsHostView),
				.WsPort = std::string(wsPortView),
				.ApiKey = std::string(apiView),
				.SecretKey = std::string(secretKeyView),
				.Cap = cap,
				.BaseDelay = baseDelay,
				.JitterMax = jitterMax
			};
		}
		catch (const std::exception& ex)
		{
			std::cerr << "Error happened: " << ex.what() << '\n';

			return BinanceConfig {};
		}
	}
}
