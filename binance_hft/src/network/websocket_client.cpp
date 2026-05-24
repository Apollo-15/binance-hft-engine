#include "network/websocket_client.hpp"

#include <iostream>
#include <boost/asio/io_context.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <boost/beast/core/buffers_to_string.hpp>

void Binance::WebSocketClient::Connect()
{
	CurrentStatus = ConnectionStatus::Connecting;

	Resolver.async_resolve(
		Host, 
		Port,
		[self = shared_from_this()](
			boost::system::error_code ec,
			const Tcp::resolver::results_type& results
			)
		{
			if (ec)
			{
				self->CurrentStatus = ConnectionStatus::Error;
				std::cout << "Resolve error: " << ec.message() << "\n";

				return;
			}

			Net::async_connect(
				self->WebSocket.next_layer(),
				results,
				[self](
					boost::system::error_code ec,
					[[maybe_unused]]
					const Tcp::endpoint& endpoint
					)
				{
					if (ec)
					{
						self->CurrentStatus = ConnectionStatus::Error;
						std::cout << "Connection error: " << ec.message() << "\n";

						return;
					}
					
					self->WebSocket.async_handshake(
						self->Host,
						"/ws/btcusdt@trade",
						[self](
							boost::system::error_code ec
							)
						{
							if (ec)
							{
								self->CurrentStatus = ConnectionStatus::Error;
								std::cout << "Handshake error: " << ec.message() << "\n";

								return;
							}

							self->CurrentStatus = ConnectionStatus::Connected;
							std::cout << "Connection completed!" << "\n";

							self->ReadMessage();
						}
					);
				}
			);
		}
	);
}

void Binance::WebSocketClient::ReadMessage()
{
	WebSocket.async_read(
		Buffer,
		[self = shared_from_this()](
			boost::system::error_code ec,
			[[maybe_unused]]
			std::size_t bytesTransferred
			)
		{
			if (ec)
			{
				std::cout << "Reading error: " << ec.message() << "\n";

				return;
			}

			std::string const message = Beast::buffers_to_string(self->Buffer.data());

			self->Buffer.consume(self->Buffer.size());

			std::cout << message << "\n";

			self->ReadMessage();
		}
	);
}

void Binance::WebSocketClient::Close()
{
	WebSocket.async_close(
		WebSocket::close_code::normal,
		[self = shared_from_this()](
			boost::system::error_code ec
			)
		{
			if (ec)
			{
				std::cout << "Close error: " << ec.message() << "\n";

				return;
			}

			self->CurrentStatus = ConnectionStatus::Closed;
			std::cout << "Connection closed!" << "\n";
		}
	);
}

Binance::ConnectionStatus Binance::WebSocketClient::GetStatus() const
{
	
}
