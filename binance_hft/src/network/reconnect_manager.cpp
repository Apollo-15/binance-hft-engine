#include "network/reconnect_manager.hpp"
#include "network/websocket_client.hpp"

#include <cmath>
#include <algorithm>
#include <iostream>

void Binance::ReconnectManager::OnConnected()
{
	if (IsReconnecting)
	{
		IsReconnecting = false;
		ExponentialBackoff = 0;
	}
}

void Binance::ReconnectManager::StartReconnect()
{
	if (IsReconnecting)
	{
		return;
	}

	IsReconnecting = true;
	const int64_t baseSeconds = BConfig.BaseDelay.count();
	const double multiplier = std::pow(2, ExponentialBackoff);
	const double rawSeconds = multiplier * static_cast<double>(baseSeconds);
	const auto backoffDelay = std::chrono::seconds(static_cast<int64_t>(rawSeconds));
	const std::chrono::duration<int64_t> cappedDelay = std::min(backoffDelay, BConfig.Cap);

	std::uniform_int_distribution<int64_t> distribution(-BConfig.JitterMax.count(), +BConfig.JitterMax.count());

	const int64_t jitter = distribution(Range);
	const auto jitterDelay = std::chrono::seconds(jitter);
	const auto finalDelay = cappedDelay + jitterDelay;

	SteadyTimer.expires_after(finalDelay);

	SteadyTimer.async_wait([this](
		boost::system::error_code ec
		)
	{
		if (ec)
		{
			IsReconnecting = false;
			std::cerr << "An error occured: " << ec.message() << '\n';
		}
		else
		{
			const auto lockedSocket = Socket.lock();

			if (lockedSocket == nullptr)
			{
				IsReconnecting = false;
				return;
			}
			else
			{
				ExponentialBackoff++;
				IsReconnecting = false;
				lockedSocket->Reset();
				lockedSocket->Connect();
			}
		}
	});
}
