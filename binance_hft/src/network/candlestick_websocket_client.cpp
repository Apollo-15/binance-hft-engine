#include "network/candlestick_websocket_client.hpp"

#include <boost/beast/core/buffers_to_string.hpp>
#include <ftxui/component/screen_interactive.hpp>

#ifdef _WIN32
#include <debugapi.h>
#endif

#include "market/candlestick_storage.hpp"
#include "market/candlestick_websocket_path_builder.hpp"
#include "parser/candlestick_parser.hpp"

void CandlestickWebSocketClient::Connect()
{
	CurrentStatus = ConnectionStatus::Connecting;

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
				self->SetErrorMessage("A Resolver Error Occurred: " + ec.message());
				self->TechnicalLogger->error("[Candlestick WebSocket Client] A Resolver Error Occurred: {}", ec.message());

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
						self->SetErrorMessage("A Connection Error Occurred: " + ec.message());
						self->TechnicalLogger->error("[Candlestick WebSocket Client] A Connection Error Occurred: {}", ec.message());

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
								self->SetErrorMessage("A SSL Handshake Error Occurred: " + ec.message());
								self->TechnicalLogger->error("[Candlestick WebSocket Client] A SSL Handshake Error Occurred: {}", ec.message());

								auto lockedManager = self->Manager.lock();

								if (lockedManager == nullptr)
								{
									return;
								}

								lockedManager->StartReconnect();

								return;
							}

							Beast::get_lowest_layer(*self->WebSocket).expires_never();

						    WebSocket::stream_base::timeout timeoutOptions;
                            timeoutOptions.handshake_timeout = std::chrono::seconds(15);
                            timeoutOptions.idle_timeout = std::chrono::seconds(30);
                            timeoutOptions.keep_alive_pings = true;
						    self->WebSocket->set_option(timeoutOptions);

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
										self->SetErrorMessage("A Candlestick Handshake Error Occurred: " + ec.message());
										self->TechnicalLogger->error("[Candlestick WebSocket Client] A Candlestick Handshake Error Occurred: {}", ec.message());

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
									self->TechnicalLogger->info("[Candlestick WebSocket Client] Connection Established.");

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
				self->SetErrorMessage("A Candlestick Close Error Occurred: " + ec.message());
				self->TechnicalLogger->error("[Candlestick WebSocket Client] A Candlestick Close Error Occurred: {}", ec.message());

				return;
			}

			self->CurrentStatus = ConnectionStatus::Closed;

			self->TechnicalLogger->info("[Candlestick WebSocket] Connection closed.");

			self->BConfig.CWsPath = BuildCandlestickWebSocketPath(self->OwnSymbol);

			self->TechnicalLogger->debug("[Candlestick WebSocket] Path: {}", self->BConfig.CWsPath);

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
					self->SetErrorMessage("A Candlestick Reading Error Occurred: " + ec.message());
					self->TechnicalLogger->error("[Candlestick WebSocket Client] A Candlestick Read Error Occurred: {}", ec.message());

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

			const auto parsedData = JsonParser::ParseCandlestick(message, self->TechnicalLogger);
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
