#pragma once

#include <string>
#include <boost/asio/io_context.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket/stream.hpp>

#include "config/binance_config.hpp"
#include "market/symbol_utilities.hpp"
#include "models/account_info.hpp"
#include "models/candle.hpp"
#include "models/full_response.hpp"
#include "models/transaction_data.hpp"

namespace Beast = boost::beast;                  // from <boost/beast.hpp>
namespace Http = Beast::http;                   // from <boost/beast/http.hpp>
namespace WebSocket = Beast::websocket;        // from <boost/beast/websocket.hpp>
namespace Net = boost::asio;                  // from <boost/asio.hpp>
using Tcp = boost::asio::ip::tcp;            // from <boost/asio/ip/tcp.hpp>

namespace Binance
{
	class RestRequest
	{
	private:
		std::string ApiKey;
		std::string SecretKey;
		std::string Host;
		std::string Port;
		Net::io_context& IoContext;
		Net::ssl::context& SslContext;

	public:
		explicit RestRequest(std::string apiKey, std::string secretKey, std::string host, std::string port,
			Net::io_context& ioContext, Net::ssl::context& sslContext)
			: ApiKey(std::move(apiKey)),
			  SecretKey(std::move(secretKey)),
			  Host(std::move(host)),
			  Port(std::move(port)),
			  IoContext(ioContext),
			  SslContext(sslContext)
		{
		}

		~RestRequest() = default;

		RestRequest(const RestRequest& other) = delete;
		RestRequest(RestRequest&& other) noexcept = delete;
		RestRequest& operator=(const RestRequest& other) = delete;
		RestRequest& operator=(RestRequest&& other) noexcept = delete;

		AccountInfo FetchAccountInfo();
		FullResponse SendTransaction(const TransactionData& data);
		std::vector<Candle> FetchHistoricalCandlesticks(Interval interval, Symbol symbol, const std::string& restHost);
	};
}
