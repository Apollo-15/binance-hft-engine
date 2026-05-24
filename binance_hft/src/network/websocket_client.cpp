#include "network/websocket_client.hpp"

#include <iostream>
#include <boost/asio/io_context.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/websocket/stream.hpp>

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
					const Tcp::endpoint&
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
	
}

void Binance::WebSocketClient::Close()
{
	
}

Binance::ConnectionStatus Binance::WebSocketClient::GetStatus() const
{
	
}
