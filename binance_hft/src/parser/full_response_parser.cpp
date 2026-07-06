#include "parser/full_response_parser.hpp"
#include "models/full_response.hpp"

#include <../include/simdjson.h>

namespace OnDemand = simdjson::ondemand;

std::optional<FullResponse> JsonParser::ParseFullResponse(const std::string& json)
{
	try
	{
		std::string_view view;
		std::string jsonCopy = json;

		OnDemand::parser parser;
		OnDemand::document doc = parser.iterate(simdjson::pad(jsonCopy));
		
		doc["symbol"].get(view);
		std::string symbol = std::string(view);

		uint64_t orderId = doc["orderId"].get_uint64();
		int64_t orderListId = doc["orderListId"].get_int64();
		
		doc["clientOrderId"].get(view);
		std::string clientOrderId = std::string(view);

		uint64_t transactionTime = doc["transactTime"].get_uint64();
		double price = doc["price"].get_double_in_string();
		double originalQuantity = doc["origQty"].get_double_in_string();
		double executedQuantity = doc["executedQty"].get_double_in_string();
		double originalQuoteOrderQuantity = doc["origQuoteOrderQty"].get_double_in_string();
		double cumulativeQuoteQuantity = doc["cummulativeQuoteQty"].get_double_in_string();

		doc["status"].get(view);
		std::string status = std::string(view);

		doc["timeInForce"].get(view);
		std::string timeInForce = std::string(view);

		doc["type"].get(view);
		std::string type = std::string(view);

		doc["side"].get(view);
		std::string side = std::string(view);

		uint64_t workingTime = doc["workingTime"].get_uint64();
		
		doc["selfTradePreventionMode"].get(view);
		std::string selfTradePreventionMode = std::string(view);
		
		OnDemand::array fills = doc["fills"].get_array();

		std::vector<FillInfo> fillInfos;

		for (auto fill : fills)
		{
			FillInfo fillInfo;

			double fillPrice = fill["price"].get_double_in_string();
			double fillQuantity = fill["qty"].get_double_in_string();
			double fillCommission = fill["commission"].get_double_in_string();

			fill["commissionAsset"].get(view);
			std::string fillCommissionAsset = std::string(view);

			uint64_t fillTradeId = fill["tradeId"].get_uint64();

			fillInfo.Price = fillPrice;
			fillInfo.Quantity = fillQuantity;
			fillInfo.Commission = fillCommission;
			fillInfo.CommissionAsset = fillCommissionAsset;
			fillInfo.TradeId = fillTradeId;

			if (std::ranges::all_of(view, [](const unsigned char c) { return std::isalnum(c); }))
			{
				fillInfos.push_back(fillInfo);
			}
		}

		return FullResponse
		{
			.Symbol = symbol,
			.OrderId = orderId,
			.OrderListId = orderListId,
			.ClientOrderId = clientOrderId,
			.TransactionTime = transactionTime,
			.Price = price,
			.OriginalQuantity = originalQuantity,
			.ExecutedQuantity = executedQuantity,
			.OriginalQuoteOrderQuantity = originalQuoteOrderQuantity,
			.CumulativeQuoteQuantity = cumulativeQuoteQuantity,
			.Status = status,
			.TimeInForce = timeInForce,
			.Type = type,
			.Side = side,
			.WorkingTime = workingTime,
			.SelfTradePreventionMode = selfTradePreventionMode,
			.Fills = fillInfos
		};

	}
	catch (const std::exception& ex)
	{
		std::cerr << "[Full Response Parser] Error happened: " << ex.what() << '\n';
		return std::nullopt;
	}
}
