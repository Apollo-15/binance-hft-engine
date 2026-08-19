#include "rest/rest_requester.hpp"

#include <iostream>
#include <boost/beast/core/tcp_stream.hpp>
#include <openssl/hmac.h>

#include "parser/account_parser.hpp"
#include "parser/candlestick_parser.hpp"
#include "parser/full_response_parser.hpp"

namespace Beast = boost::beast;                  // from <boost/beast.hpp>
namespace Http = Beast::http;                   // from <boost/beast/http.hpp>
namespace Net = boost::asio;                  // from <boost/asio.hpp>
using Tcp = boost::asio::ip::tcp;            // from <boost/asio/ip/tcp.hpp>

AccountInfo Binance::RestRequest::FetchAccountInfo()
{
	Beast::flat_buffer buffer;

	const uint64_t localTime =  std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
	const std::string timestamp = "timestamp=" + std::to_string(localTime);

	unsigned char messageDigest[32];
	HMAC(EVP_sha256(), SecretKey.c_str(), static_cast<int>(SecretKey.size()), 
		reinterpret_cast<const unsigned char*>(timestamp.c_str()), timestamp.size(),
		messageDigest, nullptr);

	std::ostringstream messageDigestStream;

	for (const unsigned char messageDigests : messageDigest)
	{
		messageDigestStream << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(messageDigests);
	}

	const std::string signatureString = "&signature=" + messageDigestStream.str();
	std::string requestBody = timestamp + signatureString;
	std::string target = "/api/v3/account?" + requestBody;

	Tcp::resolver resolver(IoContext);
	Tcp::resolver::results_type type = resolver.resolve(Host, Port);

	auto webSocket = Net::ssl::stream<Beast::tcp_stream>(IoContext, SslContext);

	if (!SSL_set_tlsext_host_name(webSocket.native_handle(), Host.c_str()))
	{
		throw Beast::system_error(
			static_cast<int>(::ERR_get_error()),
			Net::error::get_ssl_category());
	}

	Beast::get_lowest_layer(webSocket).connect(type);
	webSocket.handshake(Net::ssl::stream_base::client);

	auto httpRequest = Http::request<Http::empty_body>(Http::verb::get, target, 11);
	httpRequest.set("X-MBX-APIKEY", ApiKey);
	httpRequest.set(Http::field::host, Host);
	httpRequest.keep_alive(false);

	Http::write(webSocket, httpRequest);
	auto httpResponse = Http::response<Http::basic_string_body<char>>();
	Http::read(webSocket, buffer, httpResponse);

	httpResponse.body();

	std::optional <AccountInfo> info = JsonParser::ParseAccount(httpResponse.body());

	if (info.has_value())
	{
		return info.value();
	}
	else
	{
		return AccountInfo();
	}
}

FullResponse Binance::RestRequest::SendTransaction(const TransactionData& data)
{
	Beast::flat_buffer buffer;

	const uint64_t localTime = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
	const std::string timestamp = "&timestamp=" + std::to_string(localTime);

	std::ostringstream requestParamsStream;
	requestParamsStream << "symbol=" << data.Symbol 
						<< "&side=" << data.Side 
						<< "&type=" << data.Type 
						<< "&quantity=" << std::to_string(data.Quantity);

	const std::string linkedString =  requestParamsStream.str() + timestamp;

	unsigned char messageDigest[32];
	HMAC(EVP_sha256(), SecretKey.c_str(), static_cast<int>(SecretKey.size()),
		reinterpret_cast<const unsigned char*>(linkedString.c_str()), linkedString.size(),
		messageDigest, nullptr);

	std::ostringstream messageDigestStream;

	for (const unsigned char messageDigests : messageDigest) 
	{
		messageDigestStream << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(messageDigests);
	}

	const std::string signatureString = "&signature=" + messageDigestStream.str();
	std::string requestBody = linkedString + signatureString;
	std::string target = "/api/v3/order";


	Tcp::resolver resolver(IoContext);
	Tcp::resolver::results_type type = resolver.resolve(Host, Port);

	auto webSocket = Net::ssl::stream<Beast::tcp_stream>(IoContext, SslContext);

	if (!SSL_set_tlsext_host_name(webSocket.native_handle(), Host.c_str()))
	{
		throw Beast::system_error(
			static_cast<int>(::ERR_get_error()),
			Net::error::get_ssl_category());
	}

	Beast::get_lowest_layer(webSocket).connect(type);
	webSocket.handshake(Net::ssl::stream_base::client);
	auto httpRequest = Http::request<Http::string_body>(Http::verb::post, target, 11);
	httpRequest.set("X-MBX-APIKEY", ApiKey);
	httpRequest.set(Http::field::host, Host);
	httpRequest.set(Http::field::content_type, "application/x-www-form-urlencoded");
	httpRequest.keep_alive(false);
	httpRequest.body() = requestBody;
	httpRequest.prepare_payload();

	Http::write(webSocket, httpRequest);
	auto httpResponse = Http::response<Http::basic_string_body<char>>();
	Http::read(webSocket, buffer, httpResponse);
	//std::cout << httpResponse.body() << '\n';

	std::optional <FullResponse> info = JsonParser::ParseFullResponse(httpResponse.body());

	if (info.has_value())
	{
		/*std::cout << "Symbol: " << info->Symbol
			<< " | OrderId: " << info->OrderId
			<< " | OrderListId: " << info->OrderListId
			<< " | ClientOrderId: " << info->ClientOrderId
			<< " | TransactionTime: " << info->TransactionTime
			<< " | Price: " << info->Price
			<< " | OriginalQuantity: " << info->OriginalQuantity
			<< " | ExecutedQuantity: " << info->ExecutedQuantity
			<< " | OriginalQuoteOrderQuantity: " << info->OriginalQuoteOrderQuantity
			<< " | CumulativeQuoteQuantity: " << info->CumulativeQuoteQuantity
			<< " | Status: " << info->Status
			<< " | TimeInForce: " << info->TimeInForce
			<< " | Type: " << info->Type
			<< " | Side: " << info->Side
			<< " | WorkingTime: " << info->WorkingTime
			<< " | SelfTradePreventionMode: " << info->SelfTradePreventionMode << '\n';

		for (auto fill : info->Fills)
		{
			std::cout << "Price: " << fill.Price
				<< " | Quantity: " << fill.Quantity
				<< " | Commission: " << fill.Commission
				<< " | CommissionAsset: " << fill.CommissionAsset
				<< " | TradeId: " << fill.TradeId << '\n';
		}*/

		return info.value();
	}
	else
	{
		return FullResponse();
	}
}

std::vector<Candle> Binance::RestRequest::FetchHistoricalCandlesticks(Interval interval, Symbol symbol, const std::string& restHost)
{
	Beast::flat_buffer buffer;

	Tcp::resolver resolver(IoContext);
	Tcp::resolver::results_type type = resolver.resolve(restHost, Port);

	std::string target = "/api/v3/klines?symbol="+ std::string(SymbolToString(symbol)) + "&interval=" + std::string(IntervalToString(interval)) + "&limit=100";

	auto webSocket = Net::ssl::stream<Beast::tcp_stream>(IoContext, SslContext);

	if (!SSL_set_tlsext_host_name(webSocket.native_handle(), restHost.c_str()))
	{
		throw(Beast::system_error(
			static_cast<int>(::ERR_get_error()),
			Net::error::get_ssl_category()));
	}

	Beast::get_lowest_layer(webSocket).connect(type);
	webSocket.handshake(Net::ssl::stream_base::client);

	auto httpRequest = Http::request<Http::empty_body>(Http::verb::get, target, 11);
	httpRequest.set(Http::field::host, restHost);
	httpRequest.keep_alive(false);

	Http::write(webSocket, httpRequest);
	auto httpResponse = Http::response<Http::basic_string_body<char>>();
	Http::read(webSocket, buffer, httpResponse);

	std::optional<std::vector<Candle>> info = JsonParser::ParseHistoricalCandlestick(httpResponse.body(), interval);

	if (info.has_value())
	{
		return info.value();
	}
	else
	{
		return std::vector<Candle>();
	}
}
