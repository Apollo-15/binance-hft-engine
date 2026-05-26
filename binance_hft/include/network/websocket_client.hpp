#pragma once

#include <string>
#include <memory>
#include <atomic>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/core.hpp>

#include "models/trade_event.hpp"
#include "threading/trade_queue.hpp"

namespace Beast = boost::beast;                  // from <boost/beast.hpp>
namespace Http = Beast::http;                   // from <boost/beast/http.hpp>
namespace WebSocket = Beast::websocket;        // from <boost/beast/websocket.hpp>
namespace Net = boost::asio;                  // from <boost/asio.hpp>
using Tcp = boost::asio::ip::tcp;            // from <boost/asio/ip/tcp.hpp>

namespace Binance
{
	enum class ConnectionStatus 
		: std::uint8_t
	{
		Disconnected,
		Connecting,
		Connected,
		Closed,
		Error
	};

	class WebSocketClient 
		: public std::enable_shared_from_this<WebSocketClient>
	{
	public:
		void Connect();
		void Close();
		void ReadMessage();

		[[nodiscard]]
		ConnectionStatus GetStatus() const;

		explicit WebSocketClient(std::string host, std::string port, Net::io_context& ioContext, Net::ssl::context& sslContext)
			: Host(std::move(host)),
			  Port(std::move(port)),
			  IoContext(ioContext),
			  SslContext(sslContext),
		      Resolver(ioContext),
			  WebSocket(ioContext, sslContext)
		{
		}

	private:
		std::string Host;
		std::string Port;
		Net::io_context& IoContext;
		Net::ssl::context& SslContext;
		Tcp::resolver Resolver;
		Beast::flat_buffer Buffer;
		WebSocket::stream<Beast::ssl_stream<Beast::tcp_stream>> WebSocket;

		void OnResolve();
		void OnConnect();
		void OnHandshake();
		void OnRead();

		std::atomic<ConnectionStatus> CurrentStatus { ConnectionStatus::Disconnected };

		TradeQueue<TradeEvent> Event;
	};
}