#pragma once

#include <random>
#include <memory>

#include "iwebsocket_client.hpp"
#include "boost/asio/steady_timer.hpp"
#include "config/binance_config.hpp"

namespace Binance
{
	class ReconnectManager
	{
	private:
		std::mt19937 Range;
		boost::asio::steady_timer SteadyTimer;
		bool IsReconnecting;
		size_t ErrorCode;
		std::weak_ptr<IWebSocketClient> Socket;
		uint8_t ExponentialBackoff;
		boost::asio::io_context& IoContext;
		BinanceConfig BConfig;

	public:
		explicit ReconnectManager(std::weak_ptr<IWebSocketClient> socket, boost::asio::io_context& ioContext, BinanceConfig bConfig)
			: Range(std::random_device {} ()),
			  SteadyTimer(ioContext),
			  IsReconnecting(false),
			  ErrorCode(0),
			  Socket(std::move(socket)),
			  ExponentialBackoff(0),
			  IoContext(ioContext),
		      BConfig(std::move(bConfig))

		{
		}

		void StartReconnect();
		void OnConnected();
	};
}
