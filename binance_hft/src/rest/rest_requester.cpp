#include "rest/rest_requester.hpp"

#include <iostream>
#include <boost/beast/core/tcp_stream.hpp>
#include <openssl/hmac.h>

#include "parser/account_parser.hpp"


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

	unsigned char md[32];
	HMAC(EVP_sha256(), SecretKey.c_str(), static_cast<int>(SecretKey.size()), 
		reinterpret_cast<const unsigned char*>(timestamp.c_str()), timestamp.size(),
		md, nullptr);

	std::ostringstream mdString;

	for (const unsigned char md1 : md)
	{
		mdString << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(md1);
	}

	const std::string specialString = "&signature=" + mdString.str();
	std::string linkedString = timestamp + specialString;
	std::string target = "/api/v3/account?" + linkedString;

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
