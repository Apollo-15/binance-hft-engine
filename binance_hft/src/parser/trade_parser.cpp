#include "parser/trade_parser.hpp"
#include "../include/models/trade_event.hpp"
#include <../include/simdjson.h>

namespace OnDemand = simdjson::ondemand;

std::optional<TradeEvent> JsonParser::Parse(const std::string& json)
{
	try
	{
		std::string_view view;
		std::string jsonCopy = json;
		OnDemand::parser parser;
		OnDemand::document doc = parser.iterate(simdjson::pad(jsonCopy));

		const double price = doc["p"].get_double_in_string();
		const double quantity = doc["q"].get_double_in_string();
		const int64_t tradeTime = doc["T"].get_int64();
		const int64_t tradeId = doc["t"].get_int64();
		doc["s"].get(view);

		return TradeEvent 
		{ 
			.Price = price, 
			.Quantity = quantity, 
			.TradeTime = tradeTime, 
			.TradeId = tradeId, 
			.Symbol = std::string(view)
		};
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Error happened: " << ex.what() << '\n';

		return std::nullopt;
	}
}