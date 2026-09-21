#include "network/candlestick_websocket_client.hpp"

#include <boost/beast/core/buffers_to_string.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <debugapi.h>

#include "market/candlestick_storage.hpp"
#include "market/candlestick_websocket_path_builder.hpp"
#include "parser/candlestick_parser.hpp"

void CandlestickWebSocketClient::Connect()
{
	CurrentStatus = ConnectionStatus::Connecting;

	if (IoThread.joinable() && IoContext.stopped())
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
				self->SetErrorMessage("Resolve error: " + ec.message());

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
						self->SetErrorMessage("Connection error: " + ec.message());

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
								self->SetErrorMessage("SSL Handshake error: " + ec.message());

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
										self->SetErrorMessage("Candlestick Handshake error: " + ec.message());

										auto lockedManager = self->Manager.lock();

										if (lockedManager == nullptr)
										{
											return;
										}

										lockedManager->StartReconnect();

										return;
									}

									self->SetErrorMessage("");
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
		[self = shared_from_this()](
			boost::system::error_code ec
			)
		{
			if (ec && ec != boost::asio::ssl::error::stream_truncated && ec != boost::asio::error::operation_aborted)
			{
				self->SetErrorMessage("Candlestick Close Error: " + ec.message());

				return;
			}

			self->CurrentStatus = ConnectionStatus::Closed;
			// TODO: move to Logs tab via spdlog once technical logging is implemented
			std::cout << "Connection closed!" << '\n';

			self->BConfig.CWsPath = BuildCandlestickWebSocketPath(self->OwnSymbol);
			OutputDebugStringA((self->BConfig.CWsPath + "\n").c_str());
			self->Reset();
			self->Connect();
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
				if (ec != boost::asio::error::operation_aborted)
				{
					self->SetErrorMessage("Candlestick Reading error: " + ec.message());

					auto lockedManager = self->Manager.lock();

					if (lockedManager == nullptr)
					{
						return;
					}

					lockedManager->StartReconnect();

				}

				return;
			}

			std::string const message = Beast::buffers_to_string(self->Buffer.data());

			self->Buffer.consume(self->Buffer.size());

			const auto parsedData = JsonParser::ParseCandlestick(message);
			if (parsedData)
			{
				CandlestickStorage::Instance().Upsert(parsedData->Interval, self->OwnSymbol, *parsedData);
				if (ftxui::ScreenInteractive::Active() != nullptr)
				{
					ftxui::ScreenInteractive::Active()->RequestAnimationFrame();
				}
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

std::string CandlestickWebSocketClient::GetErrorMessage() const
{
	std::scoped_lock<std::mutex> lock(Mutex);

	return ErrorMessage;
}

void CandlestickWebSocketClient::SetErrorMessage(const std::string& errorMessage)
{
	std::scoped_lock<std::mutex> lock(Mutex);

	ErrorMessage = errorMessage;
}
