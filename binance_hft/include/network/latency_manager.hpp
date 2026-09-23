#pragma once

#include "rest/rest_requester.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <atomic>
#include <chrono>
#include <thread>

namespace Beast = boost::beast;                  // from <boost/beast.hpp>
namespace Http = Beast::http;                   // from <boost/beast/http.hpp>
namespace WebSocket = Beast::websocket;        // from <boost/beast/websocket.hpp>
namespace Net = boost::asio;                  // from <boost/asio.hpp>
using Tcp = boost::asio::ip::tcp;            // from <boost/asio/ip/tcp.hpp>

class LatencyManager 
{
private:
	Binance::RestRequest& RestRequesterRef;
	const std::string RestHost;
	Net::io_context LatencyIoContext;
	std::thread LatencyManagerThread;
	std::atomic<std::chrono::milliseconds> Latency { std::chrono::milliseconds(0) } ;
	std::atomic<unsigned int> ResultCode { 0 } ;
	std::atomic<bool> BIsRunning { false };
	std::chrono::milliseconds BaseInterval { std::chrono::milliseconds(150) };
	std::chrono::milliseconds SleepInterval { BaseInterval };
	std::chrono::milliseconds CapInterval { std::chrono::milliseconds(5000) };

	std::pair<std::chrono::milliseconds, unsigned int> MeasureLatency();

public:
	explicit LatencyManager(Binance::RestRequest& restRequesterRef, const std::string& restHost)
		: RestRequesterRef(restRequesterRef),
	      RestHost(restHost)
	{
	}

	~LatencyManager();

	LatencyManager(const LatencyManager& other) = delete;
	LatencyManager(LatencyManager&& other) = delete;
	LatencyManager& operator=(const LatencyManager& other) = delete;
	LatencyManager& operator=(LatencyManager&& other) = delete;

	void StartMeasuringLatency();

	std::chrono::milliseconds GetLatency() const { return Latency; }
	unsigned int GetResultCode() const { return ResultCode; }
};