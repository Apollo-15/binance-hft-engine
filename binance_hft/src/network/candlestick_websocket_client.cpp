#include "network/candlestick_websocket_client.hpp"

#include <boost/beast/core/buffers_to_string.hpp>

#include "market/candlestick_storage.hpp"
#include "parser/candlestick_parser.hpp"


void CandlestickWebSocketClient::Connect()
{
	CurrentStatus = ConnectionStatus::Connecting;

	if (IoThread.joinable())
	{
		IoContext.restart();
	}

	Resolver.async_resolve(
		BConfig.WsHost,
		BConfig.WsPort,
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
								self->BConfig.WsHost,
								self->BConfig.CWsPath,
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

									self->ReadMessage();
								}
							);
						}
					);
				}
			);
		}
	);


	if (!IoThread.joinable())
	{
		IoThread = std::thread([this] { IoContext.run(); });
	}
}

void CandlestickWebSocketClient::Close()
{
	WebSocket->async_close(
		WebSocket::close_code::normal,
		[this](
			boost::system::error_code ec
			)
		{
			if (ec)
			{
				std::cerr << "Candlestick Close Error: " << ec.message();

				return;
			}

			this->CurrentStatus = ConnectionStatus::Closed;
			std::cout << "Connection closed!" << '\n';
		}
	);
}

void CandlestickWebSocketClient::ReadMessage()
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

			const auto parsedData = JsonParser::ParseCandlestick(message);
			if (parsedData)
			{
				CandlestickStorage::Instance().Upsert(parsedData->Interval, *parsedData);
			}

			self->ReadMessage();
		}
	);
}

void CandlestickWebSocketClient::Reset()
{
	WebSocket = std::make_unique<WebSocket::stream<Beast::ssl_stream<Beast::tcp_stream>>>(IoContext, SslContext);
}

ConnectionStatus CandlestickWebSocketClient::GetStatus() const
{
	return CurrentStatus;
}

void CandlestickWebSocketClient::SetManager(const std::weak_ptr<Binance::ReconnectManager>& manager)
{
	Manager = manager;
}
