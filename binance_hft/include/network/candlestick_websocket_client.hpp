#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <../include/simdjson.h>
#include <thread>
#include <memory>

#include "boost/asio/steady_timer.hpp"
#include "config/binance_config.hpp"
#include "market/symbol_utilities.hpp"
#include "models/connection_status.hpp"
#include "network/iwebsocket_client.hpp"
#include "network/reconnect_manager.hpp"

namespace Beast = boost::beast;                  // from <boost/beast.hpp>
namespace Http = Beast::http;                   // from <boost/beast/http.hpp>
namespace WebSocket = Beast::websocket;        // from <boost/beast/websocket.hpp>
namespace Net = boost::asio;                  // from <boost/asio.hpp>
using Tcp = boost::asio::ip::tcp;            // from <boost/asio/ip/tcp.hpp>

namespace OnDemand = simdjson::ondemand;   // from <../include/simdjson.h>

class CandlestickWebSocketClient 
	: public std::enable_shared_from_this<CandlestickWebSocketClient>,
	  public IWebSocketClient
{
private:
	Net::io_context IoContext;
	Net::ssl::context& SslContext;
	Tcp::resolver Resolver;
	Beast::flat_buffer Buffer;
	std::unique_ptr<WebSocket::stream<Beast::ssl_stream<Beast::tcp_stream>>> WebSocket;
	OnDemand::parser Parser;
	std::thread IoThread;
	ConnectionStatus CurrentStatus { ConnectionStatus::Disconnected };
	std::mt19937 Range;
	Net::steady_timer SteadyTimer;
	bool IsReconnecting;
	uint8_t ExponentialBackoff;
	Binance::BinanceConfig BConfig;
	const Symbol OwnSymbol;

	std::weak_ptr<Binance::ReconnectManager> Manager;

	void ReadMessage();

public:
	explicit CandlestickWebSocketClient(Binance::BinanceConfig bConfig, Net::ssl::context& sslContext, const Symbol ownSymbol)
		: SslContext(sslContext),
		  Resolver(IoContext),
		  WebSocket(std::make_unique<WebSocket::stream<Beast::ssl_stream<Beast::tcp_stream>>>(IoContext, sslContext)),
	      Range(std::random_device{} ()),
	      SteadyTimer(IoContext),
	      IsReconnecting(false),
	      ExponentialBackoff(0),
	      BConfig(std::move(bConfig)),
		  OwnSymbol(ownSymbol)
	{
	}

	CandlestickWebSocketClient(const CandlestickWebSocketClient& other) = delete;
	CandlestickWebSocketClient& operator=(const CandlestickWebSocketClient& other) = delete;

	[[nodiscard]]
	ConnectionStatus GetStatus() const;

	void Connect() override;
	void Close();
	void Reset() override;

	void SetManager(const std::weak_ptr<Binance::ReconnectManager>& manager);
};