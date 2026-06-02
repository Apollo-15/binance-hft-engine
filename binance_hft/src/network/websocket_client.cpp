#include "network/websocket_client.hpp"
#include "network/reconnect_manager.hpp"

#include <iostream>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <boost/beast/core/buffers_to_string.hpp>
#include <iomanip>

#include "parser/trade_parser.hpp"

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

				auto lockedManager = self->Manager.lock();

				if (lockedManager == nullptr)
				{
					return;
				}

				lockedManager->StartReconnect();

				return;
			}

			Beast::get_lowest_layer(*self->WebSocket).async_connect(
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

						auto lockedManager = self->Manager.lock();

						if (lockedManager == nullptr)
						{
							return;
						}

						lockedManager->StartReconnect();

						return;
					}

					self->WebSocket->next_layer().async_handshake(
						Net::ssl::stream_base::client,
						[self](
							boost::system::error_code ec
							)
						{
							if (ec)
							{
								self->CurrentStatus = ConnectionStatus::Error;
								std::cout << "SSL Handshake error: " << ec.message() << "\n";

								auto lockedManager = self->Manager.lock();

								if (lockedManager == nullptr)
								{
									return;
								}

								lockedManager->StartReconnect();

								return;
							}

							Beast::get_lowest_layer(*self->WebSocket).expires_never();

							self->WebSocket->async_handshake(
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

										auto lockedManager = self->Manager.lock();

										if (lockedManager == nullptr)
										{
											return;
										}

										lockedManager->StartReconnect();

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
	);
}

void Binance::WebSocketClient::ReadMessage()
{
	WebSocket->async_read(
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

				auto lockedManager = self->Manager.lock();

				if (lockedManager == nullptr)
				{
					return;
				}

				lockedManager->StartReconnect();

				return;
			}

			std::string const message = Beast::buffers_to_string(self->Buffer.data());

			self->Buffer.consume(self->Buffer.size());

			auto parsedData = JsonParser::Parse(message);

			if (parsedData)
			{
				std::time_t time = parsedData->TradeTime / 1000;
				std::tm localTime;

				(void)localtime_s(&localTime, &time);

				std::cout << "Trade ID: " << parsedData->TradeId
						  << " |Symbol: " << parsedData->Symbol 
						  << " | Price: " << parsedData->Price
				          << " | Quantity: " << std::fixed << std::setprecision(8) << parsedData->Quantity 
						  << " | Time: " << std::put_time(&localTime, "%H:%M:%S")
				          << "\n";

				self->Event.Push(parsedData.value());
			}

			self->ReadMessage();
		}
	);
}

void Binance::WebSocketClient::Close()
{
	WebSocket->async_close(
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

void Binance::WebSocketClient::Reset()
{
	WebSocket = std::make_unique<WebSocket::stream<Beast::ssl_stream<Beast::tcp_stream>>>(IoContext, SslContext);
}

Binance::ConnectionStatus Binance::WebSocketClient::GetStatus() const
{
	return CurrentStatus;
}

void Binance::WebSocketClient::SetManager(const std::weak_ptr<ReconnectManager>& manager)
{
	Manager = manager;
}