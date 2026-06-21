#include "parser/account_parser.hpp"
#include "models/account_info.hpp"

#include "../include/models/trade_event.hpp"
#include <../include/simdjson.h>

namespace OnDemand = simdjson::ondemand;

std::optional<AccountInfo> JsonParser::ParseAccount(const std::string& json)
{
	try
	{
		std::string_view view;
		std::string jsonCopy = json;
		OnDemand::parser parser;
		OnDemand::document doc = parser.iterate(simdjson::pad(jsonCopy));

		const uint64_t uId = doc["uid"].get_uint64();
		OnDemand::array positions = doc["balances"].get_array();
		std::vector<Position> accountPositon;

		for (auto position : positions)
		{
			Position userPosition;

			position["asset"].get(view);
			const double free = position["free"].get_double_in_string();

			userPosition.Symbol = std::string(view);
			userPosition.Quantity = free;
			userPosition.Price = 0.0;
			userPosition.TotalQuantity = 0.0;
			userPosition.TotalValue = 0.0;

			if (std::ranges::all_of(view, [](const unsigned char c) { return std::isalnum(c); }))
			{
				accountPositon.push_back(userPosition);
			}
		}

		return AccountInfo
		{
			.AccountId = uId,
			.Positions = accountPositon
		};
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Error happened: " << ex.what() << '\n';

		return std::nullopt;
	}
}
