#ifndef WEBSOCKET_CLIENT.HPP
#define WEBSOCKET_CLIENT.HPP

#include <string>
#include <cstdint>
#include <memory>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/websocket/stream.hpp>

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

		explicit WebSocketClient(std::string host, std::string port, Net::io_context& ioContext)
			: Host(std::move(host)),
			  Port(std::move(port)),
			  IoContext(ioContext),
			  Resolver(ioContext),
			  WebSocket(ioContext)
		{
		}

	private:
		std::string Host;
		std::string Port;
		Net::io_context& IoContext;
		Tcp::resolver Resolver;
		Beast::flat_buffer Buffer;
		WebSocket::stream<boost::asio::ip::tcp::socket> WebSocket;

		void OnResolve();
		void OnConnect();
		void OnHandshake();
		void OnRead();

		ConnectionStatus CurrentStatus = ConnectionStatus::Disconnected;
	};
}

#endif
